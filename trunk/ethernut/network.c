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

/* 
 * Initialize network and discovery hijinks
 */
void network_init() {
	log(1, "network_init()");

	//NutRegisterDevice
	//NutDhcpIfconfig
	
	/*
	if ((nuts = malloc(sizeof(nut_t))) == NULL) {
		log(0, "network_init malloc() failed: %s", strerror(errno));
		pthread_exit(NULL);
	}
	*/

	nuts = NULL;
	nut_count = 0;
	
	if (network_send_discover() < 0) {
		/* Something failed trying to send a discover packet */
	}

	network_start_thread();
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
						(struct sockaddr *)&destaddr, sizeof(destaddr));

	if (bytes < 0) {
		log(0, "discovery sendto() failed: %s", strerror(errno));
		pthread_exit(NULL);
	}

	close(sock);

	return bytes;
}

void network_start_thread() {
	pthread_t discoverythread;
	pthread_t pingthread;
	
	if (pthread_create(&discoverythread, NULL, (void *)&network_thread, NULL) != 0) {
		log(0, "discovery pthread_create failed: %s", strerror(errno));
	}
	if (pthread_create(&pingthread, NULL, (void *)&network_pingthread, NULL) != 0) {
		log(0, "pingthread pthread_create failed: %s", strerror(errno));
	}

}

void network_thread(void *args) {
	int sockopt = 1; 
	int discovery = -1;
	struct sockaddr_in listenaddr;
	struct sockaddr_in srcaddr;

	/* Is this necessary? Meh.. can't hurt */
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
	listenaddr.sin_addr.s_addr = INADDR_ANY;
	memset(&(listenaddr.sin_zero), '\0', 8);

	if (bind(discovery, (struct sockaddr *)&listenaddr, sizeof(struct sockaddr)) == -1) {
		log(0, "network_thread bind() failed: %s", strerror(errno));
		pthread_exit(NULL);
	}

	for (;;) {
		int fromlen = sizeof(srcaddr);
		int bytes = 0;
		char buf[1024];
		memset(buf, 0, 1024);

		/* Now check if we have any discovery packets coming in */
		if (recvfrom(discovery, buf, 1024, 0, (struct sockaddr *)&srcaddr, &fromlen) < 0) {
			log(0, "network_thread recvfrom() failed: %s", strerror(errno));
			pthread_exit(NULL);
		}

		log(5, "Packet from 0x%08x %s: %s", srcaddr.sin_addr, inet_ntoa(srcaddr.sin_addr), buf);

		if (strcmp(buf,DISCOVERY_MESSAGE) == 0) {
			log(20, "Packet is a discovery broadcast");
			/* Respond to this discovery with an ack to the DISCOVERY_PORT */
			srcaddr.sin_port = htons(DISCOVERY_PORT);
			log(10, "Sending 'ACK' to %s", inet_ntoa(srcaddr.sin_addr));
			if ((bytes = sendto(discovery, DISCOVERY_ACK, strlen(DISCOVERY_ACK), 0, 
									  (struct sockaddr *)&srcaddr, sizeof(struct sockaddr))) < 0) {
				log(0, "network_thread sendto() ack failed: %s", strerror(errno));
				pthread_exit(NULL);
			}

			network_addnut(srcaddr.sin_addr);
		} 
		else if (strcmp(buf, DISCOVERY_ACK) == 0) {
			log(20, "ACK received from %s", inet_ntoa(srcaddr.sin_addr));
			network_addnut(srcaddr.sin_addr);
		}
	}
}

void network_pingthread(void *args) {
	int c;
	int bytes;

	int pingfd;

	if ((pingfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		log(0, "network_thread socket() failed: %s", strerror(errno));
		pthread_exit(NULL);
	}

	for (;;) {
		/* Check the list of known ethernuts, and ping them if we haven't in DISCOVERY_INTERVAL */
		log(0, "ping - checking if I need to ping anyone");
		for (c = 0; c < nut_count; c++) {
			time_t t = time(NULL);
			log(0, "%s lastseen: %d ago", inet_ntoa(nuts[c].ip), t - nuts[c].lastseen);
			if (nuts[c].lastseen + DISCOVERY_INTERVAL < t) {
				struct sockaddr_in nutaddr;
				nutaddr.sin_family = AF_INET;
				nutaddr.sin_port = htons(DISCOVERY_PORT);
				nutaddr.sin_addr = nuts[c].ip; 
				memset(&(nutaddr.sin_zero), '\0', 8);

				log(10, "Pinging %s - haven't seen them in a while...", inet_ntoa((struct in_addr)nuts[c].ip));
				bytes = sendto(pingfd, DISCOVERY_MESSAGE, strlen(DISCOVERY_MESSAGE), 0, 
									(struct sockaddr *)&nutaddr, sizeof(nutaddr));

				if (bytes < 0) {
					log(0, "discovery ping sendto() failed: %s", strerror(errno));
					pthread_exit(NULL);
				}
			}
		}

		sleep(1);
	}
}

void network_addnut(struct in_addr ip) {
	int c;
	
	/* Check if this is a existing nut */
	for (c = 0; c < nut_count; c++) {
		if (ip.s_addr == nuts[c].ip.s_addr) {
			log(10, "Found an old nut, %s, updating lastseen", inet_ntoa(nuts[c].ip));
			nuts[c].lastseen = time(NULL);
			return;
		}
	}

	/* This must be a new nut */

	log(10, "Found an new nut, %s", inet_ntoa(ip));
	nuts = realloc(nuts, sizeof(nut_t) * (nut_count + 1));
	nuts[nut_count].ip = ip;
	nuts[nut_count].lastseen = time(NULL);
	nut_count++;
}
