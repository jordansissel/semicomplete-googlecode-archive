/* 
 * i2c.c
 *
 * $Id$
 */

#include <stdlib.h>
#include <string.h>

#ifdef ETHERNUT
#include <dev/twif.h>
#else
#include "twif.h"
#endif

#include "i2c.h"
#include "log.h"

static i2cnode_t *nodes = NULL;
static int nodecount = 0;

/* The speed of our happy I2C bus */
static const int i2c_speed = 2400;

static u_char myaddr = 0; /* ??? What address do I use? */
void i2c_init() {
	log(1, "i2c_init()");

	/* Set the port high first, or it won't work... is this a bug? */
	outb(PORTD, 1); /* or 0xff */

	/* Initialize TWI/I2C interface */
	TwInit(myaddr);

	/* Set the bus speed to the correct speed.... */
	TwIOCtl(TWI_SETSPEED, (void *)&i2c_speed);

	i2c_findslaves();

	//i2c_start_threads();
}

void i2c_findslaves() {
	byte slaveaddr = TWSLA_MIN; /* Lowest slave address */
	byte data[2];
	byte recvdata[2];
	memset(data, '\0', 2);

	data[0] = I2C_DATA_PING;

	for (; slaveaddr <= TWSLA_MAX; slaveaddr++) {
		TwMasterTransact(slaveaddr, (void *)data, I2C_DATA_LENGTH, 
							  (void *)recvdata, I2C_DATA_LENGTH, 0);
	}

}

void i2c_addnode(byte addr) {
	log(10, "Adding new I2C node: 0x%02x", addr);
	nodecount++;
	nodes = (i2cnode_t *) realloc(nodes, sizeof(i2cnode_t) * nodecount);
	nodes[nodecount - 1].addr = addr;
	nodes[nodecount - 1].status = 0;
}

