/*
 * network.c
 *
 * $Id$
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <strings.h>

#include "network.h"

/* discovery listener socket */
static int discovery;

void network_init() {
	log(1, "network_init()");

	//NutRegisterDevice
	//NutDhcpIfconfig
	
	if (network_send_discover() < 0) {
		/* Something failed trying to send a discover packet */
	}
}

/* 
 * Send out a discovery packet 
 */
int network_send_discover() {
	int sock; 
	int bytes; 
	int sockopt; 

	struct sockaddr_in destaddr; 

	log(10, "Sending broadcast discovery packet");

	/* Set up the destaddr struct (where this packet is going) */
	destaddr.sin_family = AF_INET;
	destaddr.sin_port = htons(DISCOVERY_PORT);
	destaddr.sin_addr.s_addr = 0xffffffff; /* 255.255.255.255 */
	memset(&(destaddr.sin_zero), '\0', 8);

	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		log(0, "discovery socket() failed: %s", strerror(errno));
		return sock;
	}

	/* Set this socket to allow broadcast */
	sockopt = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &sockopt, sizeof(sockopt)) < 0) {
		log(0, "discovery setsockopt() failed: %s", strerror(errno));
		return sock;
	}

	bytes = sendto(sock, DISCOVERY_MESSAGE, strlen(DISCOVERY_MESSAGE), 0, 
						(struct sockaddr *)&destaddr, sizeof(struct sockaddr));

	if (bytes < 0) {
		log(0, "discovery sendto() failed: %s", strerror(errno));
		exit(1);
	}

	close(sock);

	return bytes;
}
