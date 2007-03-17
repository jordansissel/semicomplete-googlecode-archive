/*
 * structures.h
 *
 * $Id$
 *
 */

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "../config.h"
#include "hash.h"

/* Options structure */
typedef struct options {
	int use_udp;
	int forwardmulticast;
	int forwardbroadcast;
	int forwardxbox;
	int serverport;
	char *user_filter;
	char *proxyserver;
	char *pcapdev;
} options_t;

/* Declare the proxy struct first so the xbox struct can use it */
struct proxy;

typedef struct xbox {
	MACTYPE macaddr[ETHER_ADDR_LEN];
	struct proxy *proxy; /* The IP address of the proxy this xbox is located at */
	time_t lastseen; /* Last time a packet was seen from this xbox */
} xbox_t;

/* Container for remote proxies */
typedef struct proxy {
	int ip; /* The ip address of this proxy */
	struct in_addr addr;
	int port; /* The port! */
	int fd; /* File descriptor for the connection to this proxy */
	hash_t *xboxen; /* Table containing known xboxes on this proxy */
} proxy_t;

#endif /* STRUTURES_H */
