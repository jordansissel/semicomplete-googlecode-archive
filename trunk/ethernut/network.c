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
	log(0, "network_init()");

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

	struct sockaddr_in destaddr; 

	log(0, "Setting up sockaddr_in: %d, %x, 0x%08x", AF_INET, htons(DISCOVERY_PORT), 0xffffffff);
	destaddr.sin_family = AF_INET;
	destaddr.sin_port = htons(DISCOVERY_PORT);
	destaddr.sin_addr.s_addr = 0xffffffff; /* 255.255.255.255 */
	memset(&(destaddr.sin_zero), '\0', 8);

	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		log(0, "discovery socket() failed: %s", strerror(errno));
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
