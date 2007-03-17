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
#include <sys/sysctl.h>
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

/* driver state flags (newpsm_softc.state) */
#define PSM_VALID  0x80
#define PSM_OPEN   1         /* Device is currently open */
#define PSM_ASLEEP 2         /* Nap time while we wait for data. */

#ifndef PSM_PACKETQUEUE
#define PSM_PACKETQUEUE 128
#endif

#define PSM_BUFSIZE        960
#define PSM_SMALLBUFSIZE   240

/* Macros */
#define PSM_SOFTC(unit)     ((struct newpsm_softc*)devclass_get_softc(newpsm_devclass, unit))
#define PSM_UNIT(dev)       (minor(dev) >> 1)

#define endprobe(v)  do { \
	kbdc_lock(sc->kbdc, FALSE); \
	return (v); \
} while (0)

/* Debugging sysctls */
//SYSCTL_NODE(_debug, OID_AUTO, newpsm, CTLFLAG_RD, 0, "newpsm ps/2 mouse");

static int verbose = 0;
//SYSCTL_INT(_debug_psm, OID_AUTO, loglevel, CTLFLAG_RW, &verbose, 0, "PS/2 Debugging Level");

/* mouse data buffer */
typedef struct packetbuf {
	unsigned char ipacket[16]; /* interim input buffer */
	int           inputbytes;
} packetbuf_t;

/* ring buffer */
typedef struct ringbuf {
	int count; /* # of valid elements in the buffer */
	int head; /* head of pointer */
	int tail; /* tail of pointer */
	unsigned char buf[PSM_BUFSIZE];
} ringbuf_t;

/* NEWPSM_SOFTC */
/* XXX: Document these */
struct newpsm_softc {
	int unit;                   /* newpsmX device number */
	struct cdev *dev;           /* Our friend, the device */
	struct resource *intr;      /* The interrupt resource */
	KBDC kbdc;                  /* Keyboard device doohickey */
	int config;
	int flags;
	void *ih;
	int state;
	int hwid;
	int watchdog;
	struct callout_handle callout; /* watchdog timer call out */
	struct callout_handle softcallout; /* buffer timer call out */

	int packetsize;
	packetbuf_t pqueue[PSM_PACKETQUEUE];
	int pqueue_start;
	int pqueue_end;
	ringbuf_t queue;
};

static void newpsm_identify(driver_t *, device_t);
static int newpsm_probe(device_t);
static int newpsm_attach(device_t);
static int newpsm_detach(device_t);
static int newpsm_resume(device_t);
static int newpsm_shutdown(device_t);
static void newpsm_intr(void *);

static d_open_t newpsm_open;
static d_close_t newpsm_close;
static d_read_t newpsm_read;
static d_ioctl_t newpsm_ioctl;
static d_poll_t newpsm_poll;

/* Helper Functions */
static int restore_controller(KBDC kbdc, int command_byte);
static int get_aux_id(KBDC kbdc);
//static int get_mouse_status(KBDC kbdc, int *status, int flag, int len);
static void recover_from_error(KBDC kbdc);
static int enable_aux_dev(KBDC kbdc);
static int disable_aux_dev(KBDC kbdc);
static int doopen(struct newpsm_softc *sc, int command_byte);
static void dropqueue(struct newpsm_softc *);
static void flushpackets(struct newpsm_softc *);

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

static void
newpsm_identify(driver_t *driver, device_t parent)
{
	device_t psm;
	device_t psmc;
	u_long irq;
	int unit;
  
	unit = device_get_unit(parent);

	uprintf("newpsm_identify\n");

	/* Look for a mouse on the ps/2 aux port */

	psm = BUS_ADD_CHILD(parent, KBDC_RID_AUX, "newpsm", -1);
	if (psm == NULL)
		return;

	irq = bus_get_resource_start(psm, SYS_RES_IRQ, KBDC_RID_AUX);
	if (irq > 0)  {
		uprintf("success!\n");
		return;
	} 

	/* No mouse found yet, maybe psmcpnp found it earlier */

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

static int
newpsm_probe(device_t dev)
{
	int unit;
	struct newpsm_softc *sc;
	int rid;
	int mask;
	int command_byte;
	int i; /* Used to store test_aux_port()'s return value */
	//int stat[3];

	uprintf("newpsm_probe\n");
	unit = device_get_unit(dev);
	sc = device_get_softc(dev);

	rid = KBDC_RID_AUX;
	sc->intr = bus_alloc_resource_any(dev, SYS_RES_IRQ, &rid, RF_SHAREABLE | RF_ACTIVE);

	if (sc->intr == NULL) {
		return ENXIO;
	}

	bus_release_resource(dev, SYS_RES_IRQ, rid, sc->intr);

	sc->unit = unit;
	sc->kbdc = atkbdc_open(device_get_unit(device_get_parent(dev)));

	device_set_desc(dev, "PS/2 Mouse [newpsm]");

	/* Now that we have a device to talk to , let's make sure it's a mouse? */

	if (!kbdc_lock(sc->kbdc, TRUE)) {
		uprintf("newpsm%d: unable to lock the controller.\n", unit);
		return ENXIO;
	}

	/* wipe out both keyboard and aux buffers */
	empty_both_buffers(sc->kbdc, 10);

	mask = kbdc_get_device_mask(sc->kbdc) & ~KBD_AUX_CONTROL_BITS;
	command_byte = get_controller_command_byte(sc->kbdc);

	if (verbose)
		printf("newpsm%d: current command byte %04x\n", unit, command_byte);

	if (command_byte == -1) {
		printf("newpsm%d: unable to get current command byte value.\n", unit);
		endprobe(ENXIO);
	}

	/*
	 * disable the keyboard port while probing the aux port.
	 * Also, enable the aux port so we can use it during the probe
	 */
	if (!set_controller_command_byte(sc->kbdc, 
		 KBD_KBD_CONTROL_BITS | KBD_AUX_CONTROL_BITS,
		 KBD_DISABLE_KBD_PORT | KBD_DISABLE_KBD_INT
		   | KBD_ENABLE_AUX_PORT | KBD_DISABLE_AUX_INT)) {
		/* Controller error if we end up in this block. */

		restore_controller(sc->kbdc, command_byte);
		printf("newpsm%d: unable to send the command byte.\n", unit);
		endprobe(ENXIO);
	}

	/* Enable the aux port */
	write_controller_command(sc->kbdc, KBDC_ENABLE_AUX_PORT);

	/*
	 * NOTE: `test_aux_port()' is designed to return with zero if the aux
	 * port exists and is functioning. However, some controllers appears
	 * to respond with zero even when the aux port doesn't exist. (It may
	 * be that this is only the case when the controller DOES have the aux
	 * port but the port is not wired on the motherboard.) The keyboard
	 * controllers without the port, such as the original AT, are
	 * supporsed to return with an error code or simply time out. In any
	 * case, we have to continue probing the port even when the controller
	 * passes this test.
	 *
	 * XXX: some controllers erroneously return the error code 1, 2 or 3
	 * when it has the perfectly functional aux port. We have to ignore
	 * this error code. Even if the controller HAS error with the aux
	 * port, it will be detected later...
	 * XXX: another incompatible controller returns PSM_ACK (0xfa)...
	 */

	switch((i = test_aux_port(sc->kbdc))) {
		case 1:
		case 2:
		case 3:
		case PSM_ACK:
			if (verbose)
				printf("newpsm%d: strange result for test_aux_port (%d).\n", unit, i);
			break;
		case 0:       /* No Error */
			break;
		case -1:      /* Timeout */
		default:      /* Error */
			recover_from_error(sc->kbdc);
			/* XXX: Define PSM_CONFIG_IGNPORTERROR */
			/*
			 if (sc->config & PSM_CONFIG_IGNPORTERROR);
			 break;
			 */
			restore_controller(sc->kbdc, command_byte);
			if (verbose)
				printf("newpsm%d: the aux port is not functioning (%d).\n", unit, i);
			endprobe(ENXIO);
	}

	/* XXX: Define PSM_CONFIG_NORESET */
	/*
	 if (sc->config & PSM_CONFIG_NORESET) {
	    // ...
	 } else 
	 */
	/*
	 * NOTE: some controllers appear to hang the 'keyboard' when
	 * the aux port doesn't exist and 'PSMC_RESET_DEV' is issued.
	 *
	 * Attempt to reset the controller twice: this helps pierce 
	 * through some KVM switches. The second reset is non-fatal.
	 */
	if (!reset_aux_dev(sc->kbdc)) {
		recover_from_error(sc->kbdc);
		restore_controller(sc->kbdc, command_byte);
		if (verbose)
			printf("newpsm%d: failed to reset the aux device.\n", unit);
		endprobe(ENXIO);
	} else if (!reset_aux_dev(sc->kbdc)) {
		recover_from_error(sc->kbdc);
		restore_controller(sc->kbdc, command_byte);
		if (verbose)
			printf("newpsm%d: failed to reset the aux device (2nd reset).\n", unit);
		endprobe(ENXIO);
	}

	/* 
	 * Both the aux port and the aux device is functioning, see if the
	 * device can be enabled. NOTE: when enabled, the device will start
	 * sending data; se shall immediately disable the device once we
	 * know the device can be enabled.
	 */
	if (!enable_aux_dev(sc->kbdc) || !disable_aux_dev(sc->kbdc)) {
		recover_from_error(sc->kbdc);
		restore_controller(sc->kbdc, command_byte);
		if (verbose)
			printf("newpsm%d: failed to enable the aux device.\n", unit);
		endprobe(ENXIO);
	}

	/* save the default values after reset */
	/*
		if (get_mouse_status(sc->kbdc, stat, 0, 3) >= 3) {
		} else {
		}
	*/

	sc->hwid = get_aux_id(sc->kbdc);

	if (!set_controller_command_byte(sc->kbdc,
		 KBD_KBD_CONTROL_BITS | KBD_AUX_CONTROL_BITS,
		 (command_byte & KBD_KBD_CONTROL_BITS) 
		 | KBD_DISABLE_AUX_PORT | KBD_DISABLE_AUX_INT)) {
		restore_controller(sc->kbdc, command_byte);
		printf("newpsm%d: unable to set the command byte\n", unit);
		endprobe(ENXIO);
	}

	kbdc_set_device_mask(sc->kbdc, mask | KBD_AUX_CONTROL_BITS);
	kbdc_lock(sc->kbdc, FALSE);
	return 0;
}

static int
newpsm_attach(device_t dev)
{
	int unit = device_get_unit(dev);
	struct newpsm_softc *sc = device_get_softc(dev);
	int error;
	int rid;

	sc->state = PSM_VALID;
	callout_handle_init(&sc->callout);

	/* Setup our interrupt handler */
	rid = KBDC_RID_AUX;
	sc->intr = bus_alloc_resource_any(dev, SYS_RES_IRQ, &rid,
												 RF_SHAREABLE | RF_ACTIVE);

	if (sc->intr == NULL)
		return ENXIO;

	error = bus_setup_intr(dev, sc->intr, INTR_TYPE_TTY, newpsm_intr, sc, &sc->ih);

	if (error) {
		bus_release_resource(dev, SYS_RES_IRQ, rid, sc->intr);
		return (error);
	}

	/* Set up defaults */
	sc->packetsize = 3; /* Standard PS/2 Protocol uses 3 byte packets */

	sc->dev = make_dev(&newpsm_cdevsw, NEWPSM_MKMINOR(unit, FALSE), 0, 0, 0666, "newpsm%d", unit);

	printf("newpsm%d; device id %d\n", unit, sc->hwid & 0x00ff);
	return 0;
}

static int
newpsm_detach(device_t dev)
{
	struct newpsm_softc *sc = device_get_softc(dev);
	int rid;

	uprintf("newpsm_detach\n");

	if (sc->state & PSM_OPEN)
		return EBUSY;

	uprintf("newpsm teardown\n");
	rid = KBDC_RID_AUX;

	//bus_teardown_intr(dev, sc->intr, sc->ih);
	bus_release_resource(dev, SYS_RES_IRQ, rid, sc->intr);

	uprintf("Destroying device");
	destroy_dev(sc->dev);

	return 0;
}

static int
newpsm_resume(device_t dev)
{
	uprintf("newpsm_resume\n");

	return 0;
}

static int
newpsm_shutdown(device_t dev)
{
	uprintf("newpsm_shutdown\n");
	return 0;
}

static void
newpsm_intr(void *arg)
{
	struct newpsm_softc *sc = arg;
	int c;
	packetbuf_t *pb; /* XXX: define this */

	while ((c = read_aux_data_no_wait(sc->kbdc)) != -1) {
		pb = &sc->pqueue[sc->pqueue_end];

		/* Discard this byte if the device is not yet open */
		if ((sc->state & PSM_OPEN) == 0)
			continue;
		if (pb->inputbytes < sc->packetsize)
			continue;

		c = pb->ipacket[0];
		if (++sc->pqueue_end >= PSM_PACKETQUEUE)
			sc->pqueue_end = 0;
	}
}

static int 
newpsm_open(struct cdev *dev, int flag, int fmt, struct thread *td) 
{
	int unit = PSM_UNIT(dev);
	struct newpsm_softc *sc;
	int command_byte;
	int err;
	int s;

	sc = PSM_SOFTC(unit);

	/* Make sure this device is still valid */
	if ((sc == NULL) || (sc->state & PSM_VALID) == 0)
		return (ENXIO);

	/* Disallow multiple opens */
	if (sc->state & PSM_OPEN)
		return EBUSY;

	device_busy(devclass_get_device(newpsm_devclass, unit));
	
	/* Initialize State */
	sc->packetsize = 3;
	sc->queue.count = 0;
	sc->queue.head = 0;
	sc->queue.tail = 0;
	sc->pqueue_start = 0;
	sc->pqueue_end = 0;

	flushpackets(sc);

	if (!kbdc_lock(sc->kbdc, TRUE))
		return EIO;

	s = spltty();
	command_byte = get_controller_command_byte(sc->kbdc);

	/* enable aux port and temporarily disable the keyboard */
	if ((command_byte == -1) 
		 || !set_controller_command_byte(sc->kbdc,
				kbdc_get_device_mask(sc->kbdc),
				KBD_DISABLE_KBD_PORT | KBD_DISABLE_KBD_INT
				| KBD_ENABLE_AUX_PORT | KBD_DISABLE_AUX_INT)) {
		kbdc_lock(sc->kbdc, FALSE);
		splx(s);
		log(LOG_ERR, "newpsm%d: unable to set the command byte (newpsm_open).\n", unit);
		return EIO;
	}

	splx(s);

	err = doopen(sc, command_byte);

	if (err == 0)
		sc->state |= PSM_OPEN;
	kbdc_lock(sc->kbdc, FALSE);
	return err;
}

static int 
newpsm_close(struct cdev *dev, int flag, int fmt, struct thread *td) 
{
	return 0;
}

static int 
newpsm_read(struct cdev *dev, struct uio *uio, int flag) 
{
	register struct newpsm_softc *sc = PSM_SOFTC(PSM_UNIT(dev));
	unsigned char buf[PSM_SMALLBUFSIZE];
	int error = 0;
	int s;
	int l;

	if ((sc->state & PSM_VALID) == 0)
		return EIO;

	/* block until mouse activity occured */
	s = spltty();
	while (sc->queue.count <= 0) {
		sc->state |= PSM_ASLEEP;
		error = tsleep(sc, PZERO | PCATCH, "psmrea", 0);
		sc->state &= ~PSM_ASLEEP;

		if (error) {
			splx(s);
			return error;
		} else if ((sc->state & PSM_VALID) == 0) {
			splx(s);
			return EIO;
		}
	}
	splx(s);

	while ((sc->queue.count > 0) && (uio->uio_resid > 0)) {
		s = spltty();
		l = imin(sc->queue.count, uio->uio_resid);
		if (l > sizeof(buf))
			l = sizeof(buf);
		if (l > sizeof(sc->queue.buf) - sc->queue.head) {
			bcopy(&sc->queue.buf[sc->queue.head], &buf[0],
					sizeof(sc->queue.buf) - sc->queue.head);
			bcopy(&sc->queue.buf[0],
					&buf[sizeof(sc->queue.buf) - sc->queue.head],
					l - (sizeof(sc->queue.buf) - sc->queue.head));
		} else {
			bcopy(&sc->queue.buf[sc->queue.head], &buf[0], l);
		}
		sc->queue.count -= l;
		sc->queue.head = (sc->queue.head + l) % sizeof(sc->queue.buf);
		splx(s);

		error = uiomove(buf, l, uio);
		if (error)
			break;
	}

	return error;
}

static int 
newpsm_ioctl(struct cdev *dev, u_long cmd, caddr_t addr, int flag, struct thread *td) 
{
	return 0;
}

static int 
newpsm_poll(struct cdev *dev, int events, struct thread *td) 
{
	return 0;
}

DRIVER_MODULE(newpsm, atkbdc, newpsm_driver, newpsm_devclass, 0, 0);


/* Helper Functions */

/* 
 * Try and restor the controller to the state it was at before we
 * started poking it 
 */
static int
restore_controller(KBDC kbdc, int command_byte)
{
	empty_both_buffers(kbdc, 10);

	if (!set_controller_command_byte(kbdc, 0xff, command_byte)) {
		log(LOG_ERR, "newpsm: failed to restore the keyboard controller command byte.\n");
		empty_both_buffers(kbdc, 10);
		return FALSE;
	} else {
		empty_both_buffers(kbdc, 10);
		return TRUE;
	}
}

#if 0
static int
get_mouse_status(KBDC kbdc, int *status, int flag, int len)
{
	int cmd;
	int res;
	int i;

	switch (flag) {
		case 0:
		default:
			cmd = PSMC_SEND_DEV_STATUS;
			break;
		case 1:
			cmd = PSMC_SEND_DEV_DATA;
			break;
	}

	empty_aux_buffer(kbdc, 5);

	res = send_aux_command(kbdc, cmd);
	if (verbose)
		log(LOG_DEBUG, "newpsm: SEND_AUX_DEV_%s return code %04x\n", 
			 (flag == 1) ? "DATA" : "STATUS", res);
	if (res != PSM_ACK)
		return 0;

	for (i = 0; i < len; ++i) {
		status[i] = read_aux_data(kbdc);
		if (status[i] < 0)
			break;
	}

	log(LOG_DEBUG, "newpsm: %s %02x %02x %02x\n",
		 (flag == 1) ? "data" : "status", status[0], status[1], status[2]);

	return i;
}
#endif 

static int
get_aux_id(KBDC kbdc)
{
	int res;
	int id;

	empty_aux_buffer(kbdc, 5);
	res = send_aux_command(kbdc, PSMC_SEND_DEV_ID);
	if (verbose)
		log(LOG_DEBUG, "newpsm: SEND_DEV_ID return code: %04x\n", res);

	if (res != PSM_ACK)
		return -1;

	DELAY(10000);

	id = read_aux_data(kbdc);
	if (verbose)
		log(LOG_DEBUG, "newpsm: device id: %04x\n", id);

	return id;
}

static void
recover_from_error(KBDC kbdc)
{
	/* discard anything left in the output buffer */
	empty_both_buffers(kbdc, 10);

	if (!test_controller(kbdc))
		log(LOG_ERR, "newpsm: keyboard controller failed.\n");
	if (test_kbd_port(kbdc) != 0)
		log(LOG_ERR, "newpsm: keyboard port failed.\n");
}

static int
enable_aux_dev(KBDC kbdc)
{
	int res;

	res = send_aux_command(kbdc, PSMC_ENABLE_DEV);
	log(LOG_DEBUG, "newpsm: ENABLE_DEV return code:%04x\n", res);

	return (res == PSM_ACK);
}

static int
disable_aux_dev(KBDC kbdc)
{
	int res;

	res = send_aux_command(kbdc, PSMC_DISABLE_DEV);
	log(LOG_DEBUG, "newpsm: DISABLE_DEV return code:%04x\n", res);

	return (res == PSM_ACK);
}


static int
doopen(struct newpsm_softc *sc, int command_byte) 
{
	//int stat[3];

	/* enable the mouse device*/
	if (!enable_aux_dev(sc->kbdc)) {
		recover_from_error(sc->kbdc);
		restore_controller(sc->kbdc, command_byte);
		sc->state &= ~PSM_VALID;
		log(LOG_ERR, "newpsm%d: failed to enable the device (doopen).\n", sc->unit);
		return EIO;
	}

	/* XXX: Deprecated? */
	/*
	if (get_mouse_status(sc->kbdc, stat, 0, 3) < 3)
		log(LOG_DEBUG, "newpsm%d: failed to get status (doopen).\n", sc->unit);
	*/

	/* enable the aux port and interrupt */
	if (!set_controller_command_byte(sc->kbdc,
       kbdc_get_device_mask(sc->kbdc),
       (command_byte & KBD_KBD_CONTROL_BITS)
      | KBD_ENABLE_AUX_PORT | KBD_ENABLE_AUX_INT)) {
		disable_aux_dev(sc->kbdc);
		restore_controller(sc->kbdc, command_byte);
		log(LOG_ERR, "newpsm%d: failed to enable the aux interrupt (doopen).\n", sc->unit);
		return EIO;
	}

	return 0;
}

static void
dropqueue(struct newpsm_softc *sc)
{
	sc->queue.count = 0;
	sc->queue.head = 0;
	sc->queue.tail = 0;
	/*
	if ((sc->state & PSM_SOFTARMED) != 0) {
		sc->state &= ~PSM_SOFTARMED;
		untimeout(psmsoftintr, (void *)(uintptr_t)sc, sc->softcallout);
	}
	*/
	sc->pqueue_start = sc->pqueue_end;
}

static void
flushpackets(struct newpsm_softc *sc)
{

	dropqueue(sc);
	bzero(&sc->pqueue, sizeof(sc->pqueue));
}

