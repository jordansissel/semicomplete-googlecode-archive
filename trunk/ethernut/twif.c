/*
 * twif.c
 *
 * Basic testing fake interface for TWI/I2C
 *
 * Not actually used on the ethernut itself, becuase they have drivers.
 */

#ifndef ETHERNUT

#include "twif.h"
#include "log.h"

#define NUT_WAIT_INFINITE -1

static u_char addr;
static int speed;

int TwInit(u_char sla) { /* {{{ */
	u_long speed = 2400;

	log(10, "TwInit(%d)", sla);

	TwIOCtl(TWI_SETSPEED, &speed);
	return 0;
} /* }}} */

int TwIOCtl(int req, void *conf) { /* {{{ */
	int rc = 0;
	switch (req) {
		case TWI_SETSLAVEADDRESS:
			log(10, "TwIOCtl(TWI_SETSLAVEADDRESS, %d)", *(u_char *)conf);
			addr = *(u_char *)conf;
			break;
		case TWI_GETSLAVEADDRESS:
			log(10, "TwIOCtl(TWI_GETSLAVEADDRESS, 0x08x <= %d)", conf, addr);
			*(u_char *)conf = addr;
			break;
		case TWI_SETSPEED:
			log(10, "TwIOCtl(TWI_SETSPEED, %d)", *(int *)conf);
			speed = *(u_char *)conf;
			break;
		case TWI_GETSPEED:
			log(10, "TwIOCtl(TWI_GETSPEED, 0x08x <= %d)", speed);
			*(int *)conf = speed;
			break;
		case TWI_GETSTATUS:
			break;
		case TWI_SETSTATUS:
			break;

		default:
			rc = -1;
			break;
	}

	return rc;
} /* }}} */

int TwMasterTransact(u_char sla, void *txdata, u_short txlen, void *rxdata, u_short rxsiz, u_long tmo) { /* {{{ */

	if (tmo == NUT_WAIT_INFINITE)
		log(10, "TwMasterTransact(%d, %08x, %d, %08x, %d, NUT_WAIT_INFINITE)", 
			 sla, txdata, txlen, rxdata, rxsiz);
	else
		log(10, "TwMasterTransact(%d, %08x, %d, %08x, %d, %l)", 
			 sla, txdata, txlen, rxdata, rxsiz, tmo);

	return 0;
} /* }}} */

int TwMasterError() { /*{{{*/
	log(10, "TwMasterError()");
	return 0;
} /*}}}*/

int TwSlaveListen(u_char *sla, void *rxdata, u_short rxsiz, u_long tmo) {/*{{{*/
	if (tmo == NUT_WAIT_INFINITE)
		log(10, "TwSlaveListen(0x%08x, 0x%08x, %d, NUT_WAIT_INFINITE)", sla, rxdata, rxsiz);
	else
		log(10, "TwSlaveListen(0x%08x, 0x%08x, %d, %d)", sla, rxdata, rxsiz, tmo);

	return 0;
}/*}}}*/

int TwSlaveRespond(void *txdata, u_short txlen, u_long tmo) {/*{{{*/
	if (tmo == NUT_WAIT_INFINITE)
		log(10, "TwSlaveRespond(0x%08x, %d, NUT_WAIT_INFINITE)", txdata, txlen);
	else
		log(10, "TwSlaveRespond(0x%08x, %d, %d)", txdata, txlen, tmo);

	return 0;
}/*}}}*/

int TwSlaveError() {/*{{{*/
	log(10, "TwSlaveError()");

	return 0;
}/*}}}*/

#endif
