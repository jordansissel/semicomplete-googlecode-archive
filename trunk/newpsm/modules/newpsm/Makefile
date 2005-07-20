
.PATH: ${.CURDIR}/../../dev/newpsm
#.PATH: /usr/src/sys/dev/atkbdc

#CFLAGS+=	-DDEV_ISA

KMOD=		newpsm

SRCS=		newpsm.c newpsm.h
SRCS+=		device_if.h bus_if.h opt_kbd.h

#SRCS=		psm.c psm.h
#SRCS+=		device_if.h bus_if.h isa_if.h opt_isa.h opt_psm.h opt_kbd.h

.include <bsd.kmod.mk>

