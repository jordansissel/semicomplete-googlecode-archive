/*
 * network.h
 *
 * $Id$
 */

#ifndef NETWORK_H
#define NETWORK_H

#define DISCOVERY_PORT 3000
#define DISCOVERY_MESSAGE "Hello!"
#define DISCOVERY_INTERVAL 1

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

/* network_send_discover
 * Send a discovery packet.
 */
int network_send_discover();

/* Start up the network thread */
void network_start_thread();

/* The actual network thread */
void network_thread(void *args);

/* Register a new EtherNut that was found */
void network_addnut(unsigned int ip);

#endif
