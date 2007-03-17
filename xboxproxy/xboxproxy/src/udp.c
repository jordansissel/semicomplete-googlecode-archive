/*
 * udp.c
 *
 * Send network packets over udp!
 *
 * $Id$
 */

#include "udp.h"

#include <errno.h>
#include <strings.h>

#define PACKET_LENGTH 8192

extern hash_t *proxies;
extern hash_t *xboxen;

int server; /* UDP Socket for great justice */

fd_set proxysocks;

int udp_sendpacket(proxy_t *ppt, char *packet, int packetlen, struct sockaddr *to, size_t tolen) {
	int bytes;

	bytes = sendto(ppt->fd, packet, packetlen, 0, to, tolen);
	if (bytes < 0)
		debuglog(0, "udp_sendpacket failed: %s", strerror(errno));

	return bytes;
}

int udp_recvpacket(proxy_t *ppt, char *packet, int packetlen, struct sockaddr *from, size_t *tolen) {
	int bytes;

	bytes = recvfrom(ppt->fd, packet, packetlen, 0, from, tolen);
	if (bytes < 0) 
		debuglog(0, "udp_recvpacket failed: %s", strerror(errno));

	return bytes;
}

void udp_runserver() {
	struct sockaddr_in serveraddr;

	FD_ZERO(&proxysocks);

	server = socket(PF_INET, SOCK_DGRAM, 0);

	serveraddr.sin_family = PF_INET;
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_port = htons(option.serverport);

	debuglog(5, "Binding to port %d", options.serverport);
	if (bind(server, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr)) < 0) {
		debuglog(0, "bind failed: %s", strerror(errno));
		exit(0);
	}
	
	FD_SET(server, &proxysocks);

	if (options.proxyserver != NULL) {
		udp_connect_to_proxy();
	}

	/* To infinity, and beyond? 
	 * Check for packets... forever.
	 */
	
	for (;;) { 
		struct sockaddr_in srcaddr;
		struct timeval timeout;
		int size = sizeof(struct sockaddr_in); /* sizeof(srcaddr) */
		int sel;

		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		fd_set proxycopy;

		/* Copy the proxysocks fd_set */
		proxycopy = proxysocks;

		FD_SET(server, &proxysocks);
		debuglog(150, "select: %08x", proxysocks);
		sel = select(FD_SETSIZE, &proxysocks, NULL, NULL, &timeout);
		if (sel < 0) {
			debuglog(0, "select() failed: %s", strerror(errno));
			pthread_exit(NULL);
		} else if (sel == 0) {
			debuglog(150, "select() timed out waiting for data... trying again");
			proxysocks = proxycopy;
		} else { 
			/* selected data is available */
			debuglog(15, "Data ready from %d descriptors", sel);

			if (FD_ISSET(server, &proxysocks)) {
				proxy_t p;
				p.fd = server;
				debuglog(13, "Data received on udp");

				if (udp_proxyread(&p) > 0)
					FD_SET(server, &proxysocks);
				else
					FD_CLR(server, &proxysocks);
			} else {
				FD_SET(server, &proxysocks);
			}
		}
	}
}

void udp_connect(char *hostname, int port) {
	struct sockaddr_in destaddr;
	struct hostent *hostdata;
	struct in_addr in;

	if ((hostdata = gethostbyname(proxyserver))== NULL) {
		debuglog(0, "gethostbyname() failed: %s", strerror(errno));
		pthread_exit(NULL);
	}

	/* Grab the IP this hostname is, this is an ugly oneliner */
	memcpy(&(in.s_addr),hostdata->h_addr_list[0],hostdata->h_length);

	debuglog(1, "DNS: %s is %s (%s)", proxyserver, hostdata->h_name, inet_ntoa(in));

	destaddr.sin_family = PF_INET;
	destaddr.sin_addr = in;
	destaddr.sin_port = htons(options.serverport);

	debuglog(30, "Sending UDP 'hello' packet to %s", proxyserver);
	if (sendto(server, NULL, 0, 0, (struct sockaddr *)&destaddr, sizeof(struct sockaddr)) < 0) {
		debuglog(0, "sendto() failed [while saying hello]: %s", strerror(errno));
		pthread_exit(NULL);
	}

	addproxy(&destaddr, server);
	FD_SET(sock, &proxysocks);
}

void udp_proxyread(proxy_t *ppt) {
	int bytes = 0;
	int pktlen = PACKET_LENGTH;
	struct sockaddr_in from;
	itn len = sizeof(struct sockaddr);

	char *packet;

	packet = malloc(PACKET_LENGTH);
	bytes = recvfrom(ppt->fd, packet, pktlen, 0, (struct sockaddr *)&from, &len);
	ppt = addproxy(&from, ppt->fd);

	if (bytes < 0) {
		debuglog(0, "udp_proxyread: recvfrom() failed: %s", strerror(errno));
		return bytes;
	}

	addxbox(ETHERCONV((struct ether_header *)packet)->ether_shost), ppt);
	debuglog(1, "Packet received from %s:%d.", inet_ntoa(ppt->addr), ppt->port);

	network_dump_packet(ppt, packet, bytes);

	free(packet);

	return bytes;
}
