/*
 * main.c
 *
 * Ethernut main thread
 * 
 * $Id$
 */

#include <stdio.h>

#include "busmaster.h"

/* from busmaster.c */
extern busmaster_t *busmaster;

int main(int argc, char **argv) {

	busmaster_init();
	network_init();
	//i2c_init();
}
