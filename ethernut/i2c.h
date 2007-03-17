/* 
 * i2c.h
 *
 * $Id$
 */

#ifndef I2C_H
#define I2C_h

/* So that things are somewhat more readable */
typedef u_char byte;

typedef struct i2cnode {
	byte addr; /* The address of the node */
	byte status; /* Bit array....ish thing */
} i2cnode_t;
	
#define I2C_DROPBIT 0x01
#define I2C_QUENCHBIT 0x02
#define I2C_RESPOND 0x04

#define I2C_DATA_PING ((u_char)0x00)
#define I2C_DATA_LENGTH 2

void i2c_init();
void i2c_addnode(byte addr);
void i2c_findslaves();

#endif
