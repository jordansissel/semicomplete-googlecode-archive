/* 
 * busmaster.c
 *
 * $Id$
 */

#include <stdlib.h> /* for malloc and such */

#include "busmaster.h"
#include "log.h"

busmaster_t *busmaster = NULL;
int busmastercount = 0;

/* Initialize the busmaster struct */
void busmaster_init() {
	log(1, "busmaster_init()");

	//busmaster = (busmaster_t *) malloc(sizeof(busmaster_t));

	//busmaster->addr = 0;
	//busmaster->slaves = NULL;
	//busmaster->slavecount = 0;
}
