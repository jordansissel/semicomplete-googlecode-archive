
#.PATH: ${.CURDIR}/../../dev/newpsm
.PATH: /usr/src/sys/dev/atkbdc

KMOD=		psm

SRCS=		oldpsm.c
SRCS+=		device_if.h bus_if.h isa_if.h opt_isa.h opt_psm.h opt_kbd.h

.include <bsd.kmod.mk>

