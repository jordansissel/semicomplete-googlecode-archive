/*
 * network.c
 *
 * $Id$
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>

#ifdef ETHERNUT
#include <thread.h>
#include <sys/mutex.h>
#else
#include <pthread.h>
#endif

#include <errno.h>
#include <string.h>

#include "network.h"
#include "log.h"
#include "common.h"

/* List of known nuts */
static nut_t *nuts = NULL;
static unsigned int nut_count = 0;

/* nuts mutex locker */
static MUTEX nut_mutex;

/* network_init {{{
 * Initialize network and discovery hijinks
 */
void network_init() {
	log(1, "network_init()");

	//NutRegisterDevice
	//NutDhcpIfconfig
	
	nuts = NULL;
	nut_count = 0;

	if (MUTEX_INIT(&nut_mutex) != 0) {
		log(0, "network_init MUTEX_INIT failed: %s", strerror(errno));
	}
	
	if (network_send_discover() < 0) {
		/* Something failed trying to send a discover packet */
	}

	network_start_thread();
} /* }}} */

/* network_send_discover {{{
 * Send out a discovery packet 
 */
int network_send_discover() {
	int sock; 
	int bytes; 
	int sockopt; 
	char packet[4];

	struct sockaddr_in destaddr; 

	log(10, "Sending broadcast discovery packet");

	/* Set up the destaddr struct (where this packet is going) */
	destaddr.sin_family = PF_INET;
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

	packet[0] = PACKETTYPE_DISCOVERY;

	bytes = sendto(sock, packet, 1, 0, (struct sockaddr *)&destaddr, sizeof(destaddr));

	if (bytes < 0) {
		log(0, "discovery sendto() failed: %s", strerror(errno));
		THREAD_EXIT();
	}

	close(sock);

	return bytes;
} /* }}} */

/* network_start_thread {{{
 * Start up our threads -
 * discovery thread - watches for discovery packets
 * pinger thread - pings known network entities with discovery packets
 * communication thread - tcp listener, handles normal tcp connections
 */
void network_start_thread() {
	pthread_t discoverythread;
	pthread_t pingthread;
	pthread_t commthread;
	
	log(10, "Starting discovery thread");
	if (THREAD_CREATE(&discoverythread, NULL, (void *)&network_discoverythread, NULL) != 0) {
		log(0, "discovery pthread_create failed: %s", strerror(errno));
	}
	addthread(discoverythread);

	log(10, "Starting pinger thread");
	if (THREAD_CREATE(&pingthread, NULL, (void *)&network_pingthread, NULL) != 0) {
		log(0, "pingthread pthread_create failed: %s", strerror(errno));
	}
	addthread(pingthread);

	log(10, "Starting tcp communication thread");
	if (THREAD_CREATE(&commthread, NULL, (void *)&network_communicationthread, NULL) != 0) {
		log(0, "communicationthread pthread_create failed: %s", strerror(errno));
	}
	addthread(commthread);

} /* }}} */

/* network_discoverythread (THREAD) {{{
 * Set up a udp listener socket that waits for incomming discovery broadcasts
 * It responds with PACKETTYPE_DISCOVERY_ACK.
 *
 * The macro is from common.h for POSIX and from thread.h in Nut/OS
 */
THREAD(network_discoverythread, args) {
	int sockopt = 1; 
	int discovery = -1;
	struct sockaddr_in listenaddr;
	struct sockaddr_in srcaddr;

	/* Is this necessary? Meh.. can't hurt */
	memset(&srcaddr, '\0', sizeof(srcaddr));

	if ((discovery = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		log(0, "network_thread socket() failed: %s", strerror(errno));
		THREAD_EXIT();
	}

	/* Set this socket to allow broadcast */
	sockopt = 1;
	if (setsockopt(discovery, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt)) < 0) {
		log(0, "network_thread setsockopt() failed: %s", strerror(errno));
		THREAD_EXIT();
	}

	listenaddr.sin_family = PF_INET;
	listenaddr.sin_port = htons(DISCOVERY_PORT);
	listenaddr.sin_addr.s_addr = INADDR_ANY;
	memset(&(listenaddr.sin_zero), '\0', 8);

	if (bind(discovery, (struct sockaddr *)&listenaddr, sizeof(struct sockaddr)) == -1) {
		log(0, "network_thread bind() failed: %s", strerror(errno));
		THREAD_EXIT();
	}

	for (;;) {
		int fromlen = sizeof(srcaddr);
		int bytes = 0;
		//char buf[1024];
		//memset(buf, 0, 1024);
		char buf[4];
		memset(buf, 0, 4);

		/* Now check if we have any discovery packets coming in */
		/* NOTE: Per the Bus Master Specification, UDP packets are max 1 byte. */
		if (recvfrom(discovery, buf, sizeof(buf), 0, (struct sockaddr *)&srcaddr, &fromlen) < 0) {
			log(0, "network_thread recvfrom() failed: %s", strerror(errno));
			THREAD_EXIT();
		}

		log(5, "Packet from 0x%08x %s: %s", srcaddr.sin_addr, inet_ntoa(srcaddr.sin_addr), buf);

		if (buf[0] == PACKETTYPE_DISCOVERY) {
			char packet[4];

			packet[0] = PACKETTYPE_DISCOVERY_ACK;

			log(20, "Packet is a discovery broadcast");
			/* Respond to this discovery with an ack to the DISCOVERY_PORT */
			srcaddr.sin_port = htons(DISCOVERY_PORT);
			log(10, "Sending 'ACK' to %s", inet_ntoa(srcaddr.sin_addr));
			if ((bytes = sendto(discovery, packet, 1, 0, 
									  (struct sockaddr *)&srcaddr, sizeof(struct sockaddr))) < 0) {
				log(0, "network_thread sendto() ack failed: %s", strerror(errno));
				THREAD_EXIT();
			}

			network_addnut(srcaddr.sin_addr);
		} 
		else if (buf[0] == PACKETTYPE_DISCOVERY_ACK) {
			log(20, "ACK received from %s", inet_ntoa(srcaddr.sin_addr));
			network_addnut(srcaddr.sin_addr);
		}
	}

	log(0, "network_discoverythread cleaning up...");

	close(discovery);

	THREAD_EXIT();
} /* }}} */

/* network_pingthread (THREAD) {{{
 * Loops through the list of known busmasters and pings them 
 * if they haven't been seen in DISCOVERY_INTERVAL
 * If they don't respond after DISCOVERY_MAXWAIT seconds, remove them.
 */
THREAD(network_pingthread, args) {
	int c;
	int bytes;

	int pingfd;

	if ((pingfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		log(0, "network_thread socket() failed: %s", strerror(errno));
		THREAD_EXIT();
	}

	for (;;) {
		char buf[4];
		/* Check the list of known ethernuts, and ping them if we haven't in DISCOVERY_INTERVAL */
		log(100, "ping - checking if I need to ping anyone");

		/* DEBUG */ 

		buf[0] = PACKETTYPE_DATA;
		buf[1] = 0x20; /* 32... sounds good for a test address */
		buf[2] = 'a';
		buf[3] = 'b';
		network_tcpbroadcast(buf, 4);

		for (c = 0; c < nut_count; c++) {
			time_t t = time(NULL);
			log(20, "%s lastseen: %d ago", inet_ntoa(nuts[c].ip), t - nuts[c].lastseen);
			if (t - nuts[c].lastseen > DISCOVERY_MAXWAIT) {
				network_removenut(c);
			} else if (nuts[c].lastseen + DISCOVERY_INTERVAL < t) {
				struct sockaddr_in nutaddr;
				char packet[4];

				nutaddr.sin_family = PF_INET;
				nutaddr.sin_port = htons(DISCOVERY_PORT);
				nutaddr.sin_addr = nuts[c].ip; 
				memset(&(nutaddr.sin_zero), '\0', 8);

				log(10, "Pinging %s - haven't seen them in a while...", inet_ntoa((struct in_addr)nuts[c].ip));
				packet[0] = PACKETTYPE_DISCOVERY;
				bytes = sendto(pingfd, packet, 1, 0, 
									(struct sockaddr *)&nutaddr, sizeof(nutaddr));

				if (bytes < 0) {
					log(0, "discovery ping sendto() failed: %s", strerror(errno));
					THREAD_EXIT();
				}
			}
		}

		sleep(5);
	}

	log(0, "network_pingthread cleaning up");
} /* }}} */

/* network_communicationthread (THREAD) {{{
 * Listens for incomming TCP connections.
 * New connections are sent off on their own network_tcphandler thread
 */
THREAD(network_communicationthread, args) {
	int listener;
	pthread_t handlerthread;
	struct sockaddr_in listenaddr;
	struct sockaddr_in srcaddr;

	listener = socket(PF_INET, SOCK_STREAM, 0);

	listenaddr.sin_family = PF_INET;
	listenaddr.sin_addr.s_addr = INADDR_ANY;
	listenaddr.sin_port = htons(DISCOVERY_PORT);
	memset(&(listenaddr.sin_zero), '\0', 8);

	if (bind(listener, (struct sockaddr *)&listenaddr, sizeof(struct sockaddr)) == -1) {
		log(0, "network_communicationthread bind() failed: %s", strerror(errno));
		THREAD_EXIT();
	}

	if (listen(listener, SOMAXCONN) < 0) {
		log(0, "network_communicationthread listen() failed: %s", strerror(errno));
		THREAD_EXIT();
	}

	log(0, "network_communicationthread - Listening on port %d", DISCOVERY_PORT);

	for (;;) {
		int fd;
		int size = sizeof(srcaddr);
		connection_t *newconn;

		/* NOTE: This has the potential to leak memory if we don't free() this later */
		newconn = (connection_t *)malloc(sizeof(connection_t));

		if ((fd = accept(listener, (struct sockaddr *)&srcaddr, &size)) < 0) {
			log(0, "network_communicationthread accept() failed: %s", strerror(errno));
			THREAD_EXIT();
		}

		newconn->fd = fd;
		newconn->src = srcaddr;

		log(0, "network_communicationthread: new connection from %s:%d", inet_ntoa(srcaddr.sin_addr),
			 srcaddr.sin_port);

		if (THREAD_CREATE(&handlerthread, NULL, (void *)&network_tcphandler, newconn) != 0) {
			log(0, "network_communicationthread pthread_create failed: %s", strerror(errno));
		}
		addthread(handlerthread);
	}

	log(0, "network_communicationthread cleaning up");

	close(listener);
	THREAD_EXIT();
}/*}}}*//*}}}*/

/* network_tcphandler (THREAD) {{{
 * One tcphandler thread per tcp connection.
 * Handles data from tcp clients (errors and whatnot)
 */
THREAD(network_tcphandler, args) {
	connection_t *conn = (connection_t *)args; /* The filedescriptor */
	int bytes;

	/* The Bus Master spec shows that no packets will be over 4 bytes in length */
	char buf[4];

	for (;;) {
		char pkt[10];
		memset(buf, '\0', 4);
		memset(pkt, '\0', 10);
		if ((bytes = recv(conn->fd, buf, 4, 0)) <= 0) {
			if (bytes < 0)
				log(0, "network_tcphandler recv failed: %s", strerror(errno));
			else
				log(0, "network_tcphandler client closed connection.");

			close(conn->fd);
			free(conn);
			THREAD_EXIT();
		}
		sprintf(pkt, "%02x %02x %02x %02x", (int)buf[0], (int)buf[1], (int)buf[2], (int)buf[3]);

		log(10, "Packet from %s:%d: %s (%s)", 
			 inet_ntoa(conn->src.sin_addr), conn->src.sin_port, pkt, buf);

		switch (buf[0]) {
			case PACKETTYPE_DATA:
				log(5, "DATA for node %d: 0x%02x%02x", buf[1], buf[2], buf[3]);
				break;
			case PACKETTYPE_ERROR_QUENCH:
				log(5, "QUENCH node %d", buf[1]);
				break;
			case PACKETTYPE_ERROR_NOQUENCH:
				log(5, "NOQUENCH node %d", buf[1]);
				break;
			case PACKETTYPE_REBOOT:
				log(5, "REBOOT received... I don't know what to do!");
				break;
			case PACKETTYPE_ERROR_ACK:
				log(5, "ERROR ACK from mailman for node %d: code %d", buf[2], buf[3]);
				break;
			case PACKETTYPE_ERROR_BROADCAST:
				log(5, "ERROR BROADCAST from %s for node %d: code %d",
					 inet_ntoa(conn->src.sin_addr), buf[2], buf[3]);
				break;
			default:
				log(1, "Unknown packet type: %d (%d byte message)", buf[0], bytes);
				break;
		}

	}

	free(args);
	close(conn->fd);
	free(conn);
	THREAD_EXIT();
}/*}}}*/

/* network_addnut {{{
 */
void network_addnut(struct in_addr ip) {
	int c;
	
	MUTEX_LOCK(&nut_mutex);

	/* Check if this is a existing nut */
	for (c = 0; c < nut_count; c++) {
		if (ip.s_addr == nuts[c].ip.s_addr) {
			log(10, "Found an old nut, %s, updating lastseen", inet_ntoa(nuts[c].ip));
			nuts[c].lastseen = time(NULL);
			MUTEX_UNLOCK(&nut_mutex);
			return;
		}
	}

	/* This must be a new nut, register it. */

	log(10, "Found an new nut, %s", inet_ntoa(ip));
	nuts = realloc(nuts, sizeof(nut_t) * (nut_count + 1));
	nuts[nut_count].ip = ip;
	nuts[nut_count].lastseen = time(NULL);
	nut_count++;

	MUTEX_UNLOCK(&nut_mutex);
}/*}}}*/

/* network_removenut {{{
 */
void network_removenut(int index) {
	int c;

	log(0, "Trying to lock nut_mutex");
	MUTEX_LOCK(&nut_mutex);

	log(0, "ping timeout - Removing %s from nut list", inet_ntoa(nuts[index].ip));
	for (c = index; c < nut_count - 1; c++) {
		nuts[c] = nuts[c + 1];
	}

	nut_count--;

	MUTEX_UNLOCK(&nut_mutex);
}/*}}}*/

/* network_tcpbroadcast {{{
 * Send a message over TCP to known ethernuts
 */
void network_tcpbroadcast(char *message, int bytes) {
	int c;

	log(10, "Sending a message to all nuts.");

	for (c = 0; c < nut_count; c++) {
		int sock;
		struct sockaddr_in destaddr;
		
		if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
			logerror("network_tcpbroadcast socket()");
			THREAD_EXIT();
		}

		destaddr.sin_family = PF_INET;
		destaddr.sin_addr = nuts[c].ip;
		destaddr.sin_port = htons(DISCOVERY_PORT);

		if (connect(sock, (struct sockaddr *)&destaddr, sizeof(destaddr)) < 0) {
			logerror("network_tcpbroadcast connect()");
			switch (errno) {
				case ECONNREFUSED:
				case ENETUNREACH:
				case ETIMEDOUT:
					break;
				default:
					log(10, "DYING PTHREAD_SELF: %x", pthread_self());
					THREAD_EXIT();
			}
		} else {
			/* connect() was successful! */

			log(10, "Sending '%s' to %s:%d", message, inet_ntoa(destaddr.sin_addr), DISCOVERY_PORT);
			log(10, "SENDING PTHREAD_SELF: %x", pthread_self());
			if (send(sock, message, bytes, 0) < 0) {
				logerror("network_tcpbroadcast send()");
				//THREAD_EXIT();
			}
		}
		close(sock);
	}
} /* }}} */
