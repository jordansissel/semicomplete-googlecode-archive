/*
 * main.c
 *
 * Ethernut main thread
 * 
 * $Id$
 */

#include <stdio.h>

#include "busmaster.h"
#include "network.h"
#include "i2c.h"
#include "log.h"

/* from busmaster.c */
extern busmaster_t *busmaster;

int main(int argc, char **argv) {

	set_log_level(100);

	busmaster_init();
	network_init();
	i2c_init();

	for (;;) {
		//log(0, "main says hi");
		sleep(10);
	}
	return 0;
}
