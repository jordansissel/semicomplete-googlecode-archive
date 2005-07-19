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
#include <sys/mouse.h> /* for mousemode_t */
#include <machine/bus.h>
#include <machine/resource.h>
#include <dev/atkbdc/atkbdcreg.h> /* for KBDC_RID_AUX */

#define NEWPSM_MKMINOR(unit,block) (((unit) << 1) | ((block) ? 0 : 1))
#define NEWPSM_DRIVER_NAME "newpsm"

#define PSMCPNP_DRIVER_NAME "psmcpnp"

/* driver state flags (state) */
#define PSM_VALID    0x80
#define PSM_OPEN     1  /* Device is open */
#define PSM_ASLP     2  /* Waiting for mouse data */
#define PSM_SOFTARMED      4  /* Software interrupt armed */
#define PSM_NEED_SYNCBITS  8  /* Set syncbits using next data pkt */


/* Debugging */
#ifndef PSM_DEBUG
#define PSM_DEBUG 0 /* XXX: Document this value */
#endif
#define VLOG(level, args)  \
do {           \
	if (verbose >= level)   \
		log args;   \
} while (0)

#ifndef PSM_INPUT_TIMEOUT
#define PSM_INPUT_TIMEOUT  2000000  /* 2 sec */
#endif

#ifndef PSM_PACKETQUEUE
#define PSM_PACKETQUEUE 128
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


/* NEWPSM_SOFTC */
/* XXX: Document these */
struct newpsm_softc {
	int unit;
	struct cdev *dev;

	struct resource *intr;
	KBDC kbdc;
	int config;
	int flags;
	void *ih;
	int state;
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
	//.d_flags =  D_NEEDGIANT,
	.d_open =   newpsm_open,
	.d_close =  newpsm_close,
	.d_read =   newpsm_read,
	.d_ioctl =  newpsm_ioctl,
	.d_poll =   newpsm_poll,
	.d_name =   NEWPSM_DRIVER_NAME,
};

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

	uprintf("PSMC probed\n");

	irq = bus_get_resource_start(psmc, SYS_RES_IRQ, 0);
	if (irq <= 0)
		return;

	device_printf(parent, "IRQ allocated\n");

	bus_set_resource(psm, SYS_RES_IRQ, KBDC_RID_AUX, irq, 1);
}

int
newpsm_probe(device_t dev)
{
	uprintf("newpsm_probe\n");

	return 0;
}

int
newpsm_attach(device_t dev)
{
	int unit = device_get_unit(dev);
	struct newpsm_softc *sc = device_get_softc(dev);
	sc->dev = make_dev(&newpsm_cdevsw, NEWPSM_MKMINOR(unit, FALSE), 0, 0, 0666, "newpsm%d", unit);
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
