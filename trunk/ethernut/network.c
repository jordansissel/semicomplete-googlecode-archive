/*
 * network.c
 *
 * $Id$
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>

#ifdef ETHERNUT
#include <thread.h>
#else
#include <pthread.h>
#endif

#include <errno.h>
#include <string.h>

#include "network.h"
#include "log.h"

/* discovery listener socket */
static int discovery;

void network_init() {
	log(1, "network_init()");

	//NutRegisterDevice
	//NutDhcpIfconfig
	
	network_start_thread();
	
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

	bytes = 1;
	bytes = sendto(sock, DISCOVERY_MESSAGE, strlen(DISCOVERY_MESSAGE), 0, 
						(struct sockaddr *)&destaddr, sizeof(struct sockaddr));

	if (bytes < 0) {
		log(0, "discovery sendto() failed: %s", strerror(errno));
		pthread_exit(NULL);
	}

	close(sock);

	return bytes;
}

void network_start_thread() {
	pthread_t thread;
	
	if (pthread_create(&thread, NULL, (void *)&network_thread, NULL) != 0) {
		log(0, "discovery pthread_create failed: %s", strerror(errno));
	}
}

void network_thread(void *args) {
	int sockopt = 1; 
	struct sockaddr_in listenaddr;
	struct sockaddr_in srcaddr;

	memset(&srcaddr, '\0', sizeof(srcaddr));

	if ((discovery = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		log(0, "network_thread socket() failed: %s", strerror(errno));
		pthread_exit(NULL);
	}

	/* Set this socket to allow broadcast */
	sockopt = 1;
	if (setsockopt(discovery, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt)) < 0) {
		log(0, "network_thread setsockopt() failed: %s", strerror(errno));
		pthread_exit(NULL);
	}

	listenaddr.sin_family = PF_INET;
	listenaddr.sin_port = htons(DISCOVERY_PORT);
	listenaddr.sin_addr.s_addr = INADDR_BROADCAST;
	memset(&(listenaddr.sin_zero), '\0', 8);

	if (bind(discovery, (struct sockaddr *)&listenaddr, sizeof(struct sockaddr)) == -1) {
		log(0, "network_thread bind() failed: %s", strerror(errno));
		pthread_exit(NULL);
	}

	for (;;) {
		int fromlen = 0;
		int bytes = 0;
		char buf[1024];
		memset(buf, 0, 1024);

		if (recvfrom(discovery, buf, 1024, 0, (struct sockaddr *)&srcaddr, &fromlen) < 0) {
			log(0, "network_thread recvfrom() failed: %s", strerror(errno));
			pthread_exit(NULL);
		}

		log(5, "Packet from 0x%08x %s: %s", srcaddr.sin_addr, inet_ntoa(srcaddr.sin_addr), buf);


		if (strcmp(buf,"Hello!") == 0) {
			log(20, "Packet is a discovery broadcast");
		}

		if (srcaddr.sin_addr.s_addr == 0) {
			log(0, "network_thread - ignoring 'hello' from myself");
		}
		else {
			/* Respond to this discovery with an ack to the DISCOVERY_PORT */
			srcaddr.sin_port = htons(DISCOVERY_PORT);
			log(10, "Sending 'ACK' to %s", inet_ntoa(srcaddr.sin_addr));
			if ((bytes = sendto(discovery, "ACK", strlen("ACK"), 0, 
									(struct sockaddr *)&srcaddr, sizeof(struct sockaddr))) < 0) {
				log(0, "network_thread sendto() ack failed: %s", strerror(errno));
				pthread_exit(NULL);
			}
		}
	}
}
