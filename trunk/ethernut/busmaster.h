/*
 * busmaster.h
 *
 * $Id$
 */

#ifndef BUSMASTER_H
#define BUSMASTER_H

#ifndef NULL
#define NULL 0
#endif

/* i2c addresses are 1 byte
 * so slaves need only 1 byte */
typedef char i2c_slave;

/* Bus master structure */
typedef struct busmaster {

	/* The master's address? */
	char addr;

	/* The slaves */
	i2c_slave *slaves;

	/* Total number of slaves known */
	int slavecount;

} busmaster_t;

#endif
