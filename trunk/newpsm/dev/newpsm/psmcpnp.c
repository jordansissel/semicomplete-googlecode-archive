#include <sys/cdefs.h>

#include "opt_isa.h"
#include "opt_psm.h"

#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/bus.h>
#include <sys/conf.h>
#include <sys/poll.h>
#include <sys/syslog.h>
#include <machine/bus.h>
#include <sys/rman.h>
#include <sys/selinfo.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/uio.h>

#include <sys/limits.h>
#include <sys/mouse.h>
#include <machine/resource.h>

#include <isa/isavar.h>

#include <dev/atkbdc/atkbdcreg.h>
#include <dev/atkbdc/psm.h>

#define PSMCPNP_DRIVER_NAME "psmcpnp"

/*
 * This sucks up assignments from PNPBIOS and ACPI.
 */

/*
 * When the PS/2 mouse device is reported by ACPI or PnP BIOS, it may
 * appear BEFORE the AT keyboard controller.  As the PS/2 mouse device
 * can be probed and attached only after the AT keyboard controller is
 * attached, we shall quietly reserve the IRQ resource for later use.
 * If the PS/2 mouse device is reported to us AFTER the keyboard controller,
 * copy the IRQ resource to the PS/2 mouse device instance hanging
 * under the keyboard controller, then probe and attach it.
 */

static	devclass_t			psmcpnp_devclass;

static	device_probe_t			psmcpnp_probe;
static	device_attach_t			psmcpnp_attach;

static device_method_t psmcpnp_methods[] = {
	DEVMETHOD(device_probe,		psmcpnp_probe),
	DEVMETHOD(device_attach,	psmcpnp_attach),
	
	{ 0, 0 }
};

static driver_t psmcpnp_driver = {
	PSMCPNP_DRIVER_NAME,
	psmcpnp_methods,
	1,			/* no softc */
};

static struct isa_pnp_id psmcpnp_ids[] = {
	{ 0x030fd041, "PS/2 mouse port" },		/* PNP0F03 */
	{ 0x130fd041, "PS/2 mouse port" },		/* PNP0F13 */
	{ 0x1303d041, "PS/2 port" },			/* PNP0313, XXX */
	{ 0x02002e4f, "Dell PS/2 mouse port" },		/* Lat. X200, Dell */
	{ 0x80374d24, "IBM PS/2 mouse port" },		/* IBM3780, ThinkPad */
	{ 0x81374d24, "IBM PS/2 mouse port" },		/* IBM3781, ThinkPad */
	{ 0x0190d94d, "SONY VAIO PS/2 mouse port"},     /* SNY9001, Vaio */
	{ 0x0290d94d, "SONY VAIO PS/2 mouse port"},	/* SNY9002, Vaio */
	{ 0x0390d94d, "SONY VAIO PS/2 mouse port"},	/* SNY9003, Vaio */
	{ 0x0490d94d, "SONY VAIO PS/2 mouse port"},     /* SNY9004, Vaio */
	{ 0 }
};

static int
create_a_copy(device_t atkbdc, device_t me)
{
	device_t psm;
	u_long irq;

	/* find the PS/2 mouse device instance under the keyboard controller */
	psm = device_find_child(atkbdc, PSM_DRIVER_NAME,
				device_get_unit(atkbdc));
	if (psm == NULL)
		return ENXIO;
	if (device_get_state(psm) != DS_NOTPRESENT)
		return 0;

	/* move our resource to the found device */
	irq = bus_get_resource_start(me, SYS_RES_IRQ, 0);
	bus_set_resource(psm, SYS_RES_IRQ, KBDC_RID_AUX, irq, 1);

	/* ...then probe and attach it */
	return device_probe_and_attach(psm);
}

static int
psmcpnp_probe(device_t dev)
{
	struct resource *res;
	u_long irq;
	int rid;

	if (ISA_PNP_PROBE(device_get_parent(dev), dev, psmcpnp_ids))
		return ENXIO;

	/*
	 * The PnP BIOS and ACPI are supposed to assign an IRQ (12)
	 * to the PS/2 mouse device node. But, some buggy PnP BIOS
	 * declares the PS/2 mouse device node without an IRQ resource!
	 * If this happens, we shall refer to device hints.
	 * If we still don't find it there, use a hardcoded value... XXX
	 */
	rid = 0;
	irq = bus_get_resource_start(dev, SYS_RES_IRQ, rid);
	if (irq <= 0) {
		if (resource_long_value(PSM_DRIVER_NAME,
					device_get_unit(dev), "irq", &irq) != 0)
			irq = 12;	/* XXX */
		device_printf(dev, "irq resource info is missing; "
			      "assuming irq %ld\n", irq);
		bus_set_resource(dev, SYS_RES_IRQ, rid, irq, 1);
	}
	res = bus_alloc_resource_any(dev, SYS_RES_IRQ, &rid,
				     RF_SHAREABLE);
	bus_release_resource(dev, SYS_RES_IRQ, rid, res);

	/* keep quiet */
	if (!bootverbose)
		device_quiet(dev);

	return ((res == NULL) ? ENXIO : 0);
}

static int
psmcpnp_attach(device_t dev)
{
	device_t atkbdc;
	int rid;

	/* find the keyboard controller, which may be on acpi* or isa* bus */
	atkbdc = devclass_get_device(devclass_find(ATKBDC_DRIVER_NAME),
				     device_get_unit(dev));
	if ((atkbdc != NULL) && (device_get_state(atkbdc) == DS_ATTACHED)) {
		create_a_copy(atkbdc, dev);
	} else {
		/*
		 * If we don't have the AT keyboard controller yet,
		 * just reserve the IRQ for later use...
		 * (See psmidentify() above.)
		 */
		rid = 0;
		bus_alloc_resource_any(dev, SYS_RES_IRQ, &rid, RF_SHAREABLE);
	}

	return 0;
}

DRIVER_MODULE(psmcpnp, isa, psmcpnp_driver, psmcpnp_devclass, 0, 0);
DRIVER_MODULE(psmcpnp, acpi, psmcpnp_driver, psmcpnp_devclass, 0, 0);
