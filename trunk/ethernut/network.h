/*
 * network.h
 *
 * $Id$
 */

#ifndef NETWORK_H
#define NETWORK_H

#define DISCOVERY_PORT 3000
#define DISCOVERY_MESSAGE "Hello!"

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

#endif
