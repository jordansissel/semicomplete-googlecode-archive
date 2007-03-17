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
#define DISCOVERY_MAXWAIT 10

#ifndef IGNORE_INTERVAL_ERROR
#	if DISCOVERY_INTERVAL >= DISCOVERY_MAXWAIT
#		warning "DISCOVERY_INTERVAL must be less than DISCOVERY_MAXWAIT"
#     warning "Recompile with -DIGNORE_INTERVAL_ERROR to ignore this"
//		error "DISCOVERY_INTERVAL must be less than DISCOVERY_MAXWAIT. Recompile with -DIGNORE_INTERVAL_ERROR to ignore this."
#	endif
#endif

/* A nut (Bus Master) */
typedef struct nut {
	//unsigned int ip;
	struct in_addr ip;
	time_t lastseen;
} nut_t;

/* A new tcp connection, passed off to the network_tcphandler threads */
typedef struct connection {
	int fd;
	struct sockaddr_in src;
} connection_t;


/* network_init
 * Initialize the network bits. This includes running DHCP and setting up a
 * discovery listener.
 */
void network_init();

/* Send a discovery packet. */
int network_send_discover();

/* Start up the network thread */
void network_start_thread();

/* The ethernut (busmaster) discovery thread */
THREAD(network_discoverythread, args);

/* The pinger thread */
THREAD(network_pingthread, args);

/* TCP Communication thread */
THREAD(network_communicationthread, args);

/* New connection handler, created on a per-connection basis */
THREAD(network_tcphandler, args);

/* Register a new EtherNut that was found */
void network_addnut(struct in_addr ip);

/* Remove a nut, by index */
void network_removenut(int index);

/* Broadcast a message over tcp 
 * This will only send messages to known busmasters/ethernuts */
void network_tcpbroadcast(char *message, int bytes);

#endif
