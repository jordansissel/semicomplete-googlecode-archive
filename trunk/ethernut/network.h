/*
 * network.h
 *
 * $Id$
 */

#ifndef NETWORK_H
#define NETWORK_H

#include <netinet/in.h>
#include "common.h"

#define DISCOVERY_PORT 3000
#define DISCOVERY_MESSAGE "DISCOVERY"
#define DISCOVERY_ACK "ACK"

#define PACKETTYPE_DATA            ((char)0x01)
#define PACKETTYPE_ERROR_BROADCAST ((char)0x02)
#define PACKETTYPE_ERROR_ACK       ((char)0x03)
#define PACKETTYPE_ERROR_QUENCH    ((char)0x04)
#define PACKETTYPE_ERROR_NOQUENCH  ((char)0x05)
#define PACKETTYPE_DISCOVERY       ((char)0x06)
#define PACKETTYPE_DISCOVERY_ACK   ((char)0x07)
#define PACKETTYPE_SOURCE_QUENCH   ((char)0x08)
#define PACKETTYPE_REBOOT          ((char)0x09)


/* How long to wait before repinging each nut */
#define DISCOVERY_INTERVAL 5 

/* How long to wait before not having gotten a response from a ping */
#define DISCOVERY_MAXWAIT 5

typedef struct nut {
	//unsigned int ip;
	struct in_addr ip;
	time_t lastseen;
} nut_t;

/* network_init
 * Initialize the network bits. This includes running DHCP and setting up a
 * discovery listener.
 */
void network_init();

/* Send a discovery packet. */
int network_send_discover();

/* Start up the network thread */
void network_start_thread();

/* The actual network thread */
THREAD(network_thread, args);
//void network_thread(void *args);

/* The pinger thread */
THREAD(network_pingthread, args);
//void network_pingthread(void *args);

/* Register a new EtherNut that was found */
void network_addnut(struct in_addr ip);

/* Remove a nut, by index */
void network_removenut(int index);

#endif
