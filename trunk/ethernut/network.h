/*
 * network.h
 *
 * $Id$
 */

#ifndef NETWORK_H
#define NETWORK_H

#include "common.h"

#define DISCOVERY_PORT 3000
#define DISCOVERY_MESSAGE "Hello there"
#define DISCOVERY_ACK "ACK"

/* How long to wait before repinging each nut */
#define DISCOVERY_INTERVAL 10

/* How long to wait before not having gotten a response from a ping */
#define DISCOVERY_MAXWAIT 10

typedef struct nut {
	//unsigned int ip;
	struct in_addr ip;
	time_t lastseen;
} nut_t;

/* List of known nuts */
nut_t *nuts = NULL;
unsigned int nut_count = 0;

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
