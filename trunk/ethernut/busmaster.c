/* 
 * busmaster.c
 *
 * $Id$
 */

#include "busmaster.h"

busmaster_t *busmaster;

void busmaster_init() {
	busmaster = (busmaster_t *) malloc(sizeof(busmaster_t));

	busmaster->addr = 0;
	busmaster->slaves = NULL;
	busmaster->slavecount = 0;
}

