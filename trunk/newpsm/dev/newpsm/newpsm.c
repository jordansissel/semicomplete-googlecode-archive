/*
 * New, prettier, psm?
 */

#include <sys/cdefs.h>

#include <sys/types.h>
#include <sys/module.h>
#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/param.h>
#include <sys/bus.h>
#include <sys/kernel.h>
#include <sys/conf.h>
#include <sys/rman.h> /* for RF_ macros */
#include <sys/uio.h> /* for uiomove, etc */
#include <sys/syslog.h> /* for log(9) */
#include <machine/bus.h>
#include <machine/resource.h>
#include <dev/atkbdc/atkbdcreg.h> /* for KBDC_RID_AUX */

#define NEWPSM_MKMINOR(unit,block) (((unit) << 1) | ((block) ? 0 : 1))
#define NEWPSM_DRIVER_NAME "newpsm"

#define PSMCPNP_DRIVER_NAME "psmcpnp"


/* Debugging */
#ifndef PSM_DEBUG
#define PSM_DEBUG 0 /* XXX: Document this value */
#endif

void newpsm_identify(driver_t *, device_t);
int newpsm_probe(device_t);
int newpsm_attach(device_t);
int newpsm_detach(device_t);
int newpsm_resume(device_t);
int newpsm_shutdown(device_t);

d_open_t newpsm_open;
d_close_t newpsm_close;
d_read_t newpsm_read;
d_ioctl_t newpsm_ioctl;
d_poll_t newpsm_poll;

static int verbose  = PSM_DEBUG;

static int restore_controller(KBDC kbdc, int command_byte);
static void recover_from_error(KBDC kbdc);

/* XXX: Document these */
struct newpsm_softc {
	int unit;
	struct cdev *dev;

	struct resource *intr;
	KBDC kbdc;
	int config;
	int flags;
};

static device_method_t newpsm_methods[] = {
	DEVMETHOD(device_identify, newpsm_identify),
	DEVMETHOD(device_probe,    newpsm_probe),
	DEVMETHOD(device_attach,   newpsm_attach),
	DEVMETHOD(device_detach,   newpsm_detach),
	DEVMETHOD(device_resume,   newpsm_resume),
	DEVMETHOD(device_shutdown, newpsm_shutdown),

	{ 0, 0 }
};

static devclass_t newpsm_devclass;

static driver_t newpsm_driver = {
	NEWPSM_DRIVER_NAME,
	newpsm_methods,
	sizeof(struct newpsm_softc),
};

static struct cdevsw newpsm_cdevsw = {
	.d_version =   D_VERSION,
	.d_flags =  D_NEEDGIANT,
	.d_open =   newpsm_open,
	.d_close =  newpsm_close,
	.d_read =   newpsm_read,
	.d_ioctl =  newpsm_ioctl,
	.d_poll =   newpsm_poll,
	.d_name =   NEWPSM_DRIVER_NAME,
};

#define endprobe(v)  do {                  \
	if (bootverbose)                        \
		--verbose;                           \
	kbdc_set_device_mask(sc->kbdc, mask);   \
	kbdc_lock(sc->kbdc, FALSE);             \
	return (v);                             \
} while (0)

void
newpsm_identify(driver_t *driver, device_t parent)
{
	device_t psm;
	device_t psmc;
	u_long irq;
	int unit = device_get_unit(parent);

	uprintf("newpsm_identify\n");

	psm = BUS_ADD_CHILD(parent, KBDC_RID_AUX, "newpsm", -1);
	if (psm == NULL)
		return;

	irq = bus_get_resource_start(psm, SYS_RES_IRQ, KBDC_RID_AUX);
	if (irq > 0) 
		uprintf("success!\n");

	psmc = device_find_child(device_get_parent(parent), PSMCPNP_DRIVER_NAME, unit);

	if (psmc == NULL)
		return;

	irq = bus_get_resource_start(psmc, SYS_RES_IRQ, 0);
	if (irq <= 0)
		return;

	bus_set_resource(psm, SYS_RES_IRQ, KBDC_RID_AUX, irq, 1);
}

int
newpsm_probe(device_t dev)
{
	int unit = device_get_unit(dev);
	struct newpsm_softc *sc = device_get_softc(dev);
	int rid;
	int mask;
	int command_byte;
	int i;

	uprintf("newpsm_probe\n");

	rid = KBDC_RID_AUX;
	sc->intr = bus_alloc_resource_any(dev, SYS_RES_IRQ, &rid, 
												 RF_SHAREABLE | RF_ACTIVE);
	if (sc->intr == NULL) {
		if (bootverbose)
			device_printf(dev, "unable to allocate IRQ\n");
		return (ENXIO);
	}

	bus_release_resource(dev, SYS_RES_IRQ, rid, sc->intr);

	sc->unit = unit;
	sc->kbdc = atkbdc_open(device_get_unit(device_get_parent(dev)));
	sc->config = device_get_flags(dev); /* & PSM_CONFIG_FLAGS */
	sc->flags = 0;

	device_set_desc(dev, "newpsm ps/2 mouse driver");

	if (!kbdc_lock(sc->kbdc, TRUE)) {
		printf("psm%d: unable to lock the controller.\n", unit);
		return (ENXIO);
	}

	empty_both_buffers(sc->kbdc, 10);

	mask = kbdc_get_device_mask(sc->kbdc) & ~KBD_AUX_CONTROL_BITS;
	command_byte = get_controller_command_byte(sc->kbdc);

	if (command_byte == -1) {
		/* CONTROLLER ERROR */
		printf("psm%d: unable to get the current command  byte value.\n", unit);
		endprobe(ENXIO);
	}

	if (!set_controller_command_byte(sc->kbdc,
												KBD_KBD_CONTROL_BITS | KBD_AUX_CONTROL_BITS,
												KBD_DISABLE_KBD_PORT | KBD_DISABLE_KBD_INT |
												KBD_ENABLE_AUX_PORT | KBD_DISABLE_AUX_INT)) {
		/* 
		 * This is a CONTROLLER ERROR; I don't know how to recover from this
		 */
		restore_controller(sc->kbdc, command_byte);
		printf("psm%d: unable to set the command byte.\n", unit);
		endprobe(ENXIO);
	}

	write_controller_command(sc->kbdc, KBDC_ENABLE_AUX_PORT);

	switch ((i = test_aux_port(sc->kbdc))) {
		case 1:
		case 2:
		case 3:
		case PSM_ACK:
			if (verbose)
				printf("psm%d: strange result for test aux port (%d).\n", unit, i);
		/* FALLTHROUGH */

		case 0:     /* no error */
			break;
		case -1:   /* time out */
		default:   /* error */
			recover_from_error(sc->kbdc);
			/* XXX: PSM_CONFIG_IGNPORTERROR; break */
			restore_controller(sc->kbdc, command_byte);
			if (verbose)
				printf("psm%d: the aux port is not funcitoning (%d).\n", unit, i);
			endprobe(ENXIO);
	}

	/* XXX: PSM_CONFIG_NORESET */

	/*
	 * NOTE: some controllers appears to hang the `keyboard' when the aux
	 * port doesn't exist and `PSMC_RESET_DEV' is issued.
	 *
	 * Attempt to reset the controller twice -- this helps
	 * pierce through some KVM switches. The second reset
	 * is non-fatal.
	 */
	if (!reset_aux_dev(sc->kbdc)) {
		recover_from_error(sc->kbdc);
		restore_controller(sc->kbdc, command_byte);
		if (verbose)
			printf("psm%d: failed to reset the aux device.\n", unit);
		endprobe(ENXIO);
	} else if (!reset_aux_dev(sc->kbdc)) {
		recover_from_error(sc->kbdc);
		if (verbose >= 2)
			printf("psm%d: failed to reset the aux device (2).\n", unit);
	}


	return 0;
}

int
newpsm_attach(device_t dev)
{
	int unit = device_get_unit(dev);
	struct newpsm_softc *sc = device_get_softc(dev);

	sc->dev = make_dev(&newpsm_cdevsw, NEWPSM_MKMINOR(unit, FALSE), 0, 0, 0666, "newpsm%d", unit);
	uprintf("newpsm_attach\n");

	return 0;
}

int
newpsm_detach(device_t dev)
{
	struct newpsm_softc *sc = device_get_softc(dev);

	uprintf("newpsm_detach\n");
	destroy_dev(sc->dev);

	return 0;
}

int
newpsm_resume(device_t dev)
{
	uprintf("newpsm_resume\n");

	return 0;
}

int
newpsm_shutdown(device_t dev)
{
	uprintf("newpsm_shutdown\n");
	return 0;
}

int newpsm_open(struct cdev *dev, int flag, int fmt, struct thread *td) {
	return 0;
}

int newpsm_close(struct cdev *dev, int flag, int fmt, struct thread *td) {

	return 0;
}

int newpsm_read(struct cdev *dev, struct uio *uio, int flag) {
	int err;

	err = uiomove("Hello\n", 6, uio);
	return err;
}

int newpsm_ioctl(struct cdev *dev, u_long cmd, caddr_t addr, int flag, struct thread *td) {

	return 0;
}

int newpsm_poll(struct cdev *dev, int events, struct thread *td) {

	return 0;
}

DRIVER_MODULE(newpsm, atkbdc, newpsm_driver, newpsm_devclass, 0, 0);

/* Static Functions */
static int
restore_controller(KBDC kbdc, int command_byte)
{
	empty_both_buffers(kbdc, 10);

	if (!set_controller_command_byte(kbdc, 0xff, command_byte)) {
		log(LOG_ERR, "psm: failed to restore the keyboard controller "
			 "command byte.\n");
		empty_both_buffers(kbdc, 10);
		return FALSE;
	} else {
		empty_both_buffers(kbdc, 10);
		return TRUE;
	}

}

static void
recover_from_error(KBDC kbdc)
{
	empty_both_buffers(kbdc, 10);

	if (!test_controller(kbdc))
		log(LOG_ERR, "psm: keyboard controller failed.\n");

	if (test_kbd_port(kbdc) != 0)
		log(LOG_ERR, "psm: keyboard port failed.\n");

}
