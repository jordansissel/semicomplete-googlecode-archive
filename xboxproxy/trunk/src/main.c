/* 
 *
 * $Id$
 */

#include <sys/types.h>
#include <sys/time.h>

#include <pcap.h>

#include <unistd.h>

#include <errno.h>
#include <string.h>

#ifdef __FreeBSD__
#include <netinet/in_systm.h>
#endif

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/ethernet.h>

#include <pthread.h>

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN 6
#endif

#include "log.h"
#include "hash.h"

#define SERVER_PORT 3434

/* The xbox network stuff only uses an ip of 0.0.0.1 */
static char filter_app[] = "host 0.0.0.1";

/* xbox broadcasts to FF:FF:FF:FF:FF:FF */
static char broadcastmac[ETHER_ADDR_LEN] = { 
	0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF
};

typedef struct xbox {
	u_char macaddr[ETHER_ADDR_LEN];
	int location; /* The IP address of the proxy this xbox is located at */
	time_t lastseen; /* Last time a packet was seen from this xbox */
} xbox_t;

static hash_t *xboxen = NULL;

/* Container for remote proxies */
typedef struct proxy {
	int ip; /* The ip address of this proxy */
	struct in_addr addr;
	int fd; /* File descriptor for the connection to this proxy */
	hash_t *xboxen; /* Table containing known xboxes on this proxy */
} proxy_t;

static hash_t *proxies = NULL;
static fd_set proxysocks;

static char *pcapdev;

void addxbox(u_char *macaddr, int proxyip);
void packet_handler(u_char *args, const struct pcap_pkthdr *head,
						  const u_char *packet);
void remove_proxy(proxy_t *ppt);

int comparemac(const void *k1, const void *k2) {
	return memcmp(k1, k2, ETHER_ADDR_LEN);
}

int compareip(const void *k1, const void *k2) {
	int a, b;
	a = *(int *)k1;
	b = *(int *)k2;

	return (a < b ? -1 : (a > b ? 1 : 0));
}

/* 
 * Packet handling routine.
 */
void packet_handler(u_char *args, const struct pcap_pkthdr *head,
						  const u_char *packet) {
	struct ether_header *eptr;
	u_int length;
	int c;

	eptr = (struct ether_header *) packet;


	/* Try adding this xbox to the list */
	addxbox(eptr->ether_shost, 0);

	debuglog(1, "Packet!");
	debuglog(3, "From: %s", ether_ntoa((struct ether_addr *)eptr->ether_shost));
	debuglog(3, "To: %s", ether_ntoa((struct ether_addr *)eptr->ether_dhost));

	/*
	 * If this is broadcast, send it to all known proxies
	 */
	if (memcmp(eptr->ether_dhost, broadcastmac, ETHER_ADDR_LEN) == 0) {
		hscan_t hs;
		hnode_t *node;

		debuglog(1, "BROADCAST");
		hash_scan_begin(&hs, proxies);
		while ((node = hash_scan_next(&hs))) {
			proxy_t *p = (proxy_t *)(node->hash_data);
			debuglog(3, "Sending ethernet packet to %s", inet_ntoa(p->addr));
			write(p->fd, &(head->caplen), 4); /* First 4 bytes is the size */
			write(p->fd, packet, head->caplen);
		}
	}

}

void addxbox(u_char *macaddr, int proxyip) {
	xbox_t *newbox = NULL;
	hnode_t *box = NULL;

	box = hash_lookup(xboxen, macaddr);
	if (box == NULL) {
		debuglog(1, "New xbox found: %s", ether_ntoa((struct ether_addr *)macaddr));
		debuglog(1, "\tLocation: %s", (proxyip ? "Remote" : "Local"));
		newbox = malloc(sizeof(xbox_t));
		memcpy(newbox->macaddr,macaddr,ETHER_ADDR_LEN);
		newbox->lastseen = time(NULL);
		newbox->location = proxyip;
		hash_alloc_insert(xboxen, macaddr, newbox);
	}
}

void pcap(void *args) {
	pcap_t *handle;
	struct bpf_program filter;
	char errbuf[PCAP_ERRBUF_SIZE];
	bpf_u_int32	mask, net;

	pcap_lookupnet(pcapdev, &net, &mask, errbuf);
	handle = pcap_open_live(pcapdev, BUFSIZ, 1, 1000, errbuf);

	if (handle == NULL) {
		debuglog(0, "Error trying to open %s: %s", pcapdev, errbuf);
		//return 1;
		pthread_exit(NULL);
	}

	debuglog(2, "Opened %s, listening for xbox traffic", pcapdev);

	pcap_compile(handle, &filter, filter_app, 0, net);
	pcap_setfilter(handle, &filter);
	pcap_loop(handle, -1, packet_handler, NULL);
	pcap_close(handle);

	pthread_exit(NULL);
}

void proxy(void *args) {
	int server;
	struct sockaddr_in serveraddr;

	FD_ZERO(&proxysocks);

	server = socket(PF_INET, SOCK_STREAM, 0);
	serveraddr.sin_family = PF_INET;
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_port = htons(SERVER_PORT);
	
	if (bind(server, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr)) == -1) {
		debuglog(0, "bind() failed: %s", strerror(errno));
		pthread_exit(NULL);
	}

	if (listen(server, SOMAXCONN) < 0) {
		debuglog(0, "listen() failed: %s", strerror(errno));
		pthread_exit(NULL);
	}

	debuglog(1, "Listening on port %d", SERVER_PORT);

	FD_SET(server, &proxysocks);

	for (;;) {
		int fd;
		struct sockaddr_in srcaddr;
		int size = sizeof(struct sockaddr_in); /* sizeof(srcaddr) */
		int sel;
		struct timeval timeout;
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		fd_set proxycopy;

		/* Some sort of select() thing here */

		proxycopy = proxysocks;

		FD_SET(server, &proxysocks);
		debuglog(10, "select: %08x", proxysocks);
		sel = select(FD_SETSIZE, &proxysocks, NULL, NULL, &timeout);
		if (sel < 0) {
			debuglog(0, "select() failed: %s", strerror(errno));
			pthread_exit(NULL);
		} else if (sel == 0) {
			debuglog(2, "select() timed out waiting for data... trying again");
			proxysocks = proxycopy;
		} else {
			hscan_t hs;
			hnode_t *node;

			debuglog(1, "Data ready from %d descriptors", sel);

			if (FD_ISSET(server, &proxysocks)) {
				proxy_t *newproxy;

				if ((fd = accept(server, (struct sockaddr *)&srcaddr, &size)) < 0) {
					debuglog(0, "accept() failed: %s", strerror(errno));
					pthread_exit(NULL);
				}

				newproxy = malloc(sizeof(proxy_t));
				newproxy->ip = (int) srcaddr.sin_addr.s_addr;
				newproxy->addr = srcaddr.sin_addr;
				newproxy->fd = fd;
				newproxy->xboxen = hash_create(64, comparemac, NULL);

				hash_alloc_insert(proxies, &(newproxy->ip), newproxy);
				FD_SET(fd, &proxysocks);
			} 

			/* Check known proxy connections for data */
			hash_scan_begin(&hs, proxies);
			while ((node = hash_scan_next(&hs))) {
				proxy_t *p = (proxy_t *)(node->hash_data);
				debuglog(1, "Data received from %s", inet_ntoa(p->addr));
				if (FD_ISSET(p->fd, &proxysocks)) {
					if (recv_from_proxy(p) > 0 )
						FD_SET(p->fd, &proxysocks);
					else
						FD_CLR(p->fd, &proxysocks);
				} else {
					FD_SET(p->fd, &proxysocks);
				}
			}
			debuglog(0,"Data from a connected proxy client!");
		}
	}
}

int recv_from_proxy(proxy_t *ppt) {
	int bytes;
	int pktlen = 0;

	char *packet;

	packet = malloc(1024);

	bytes = recv(ppt->fd, &pktlen, 4, 0);
	debuglog(1, "[%d] %d", bytes, pktlen);

	/* if bytes read is 0, then the connection was closed. */
	if (bytes == 0) {
		remove_proxy(ppt);
		return 0;
	}

	packet = malloc(pktlen);
	bytes = recv(ppt->fd, packet, pktlen, 0);
	if (bytes == 0) {
		remove_proxy(ppt);
		return 0;
	}

	debuglog(1, "Packet received from %s. Length: %d vs %d", inet_ntoa(ppt->addr), bytes, pktlen);

	return bytes;
}

void remove_proxy(proxy_t *ppt) {
	hscan_t hs;
	hnode_t *node;

	debuglog(1, "Removing proxy %s", inet_ntoa(ppt->addr));

	hash_scan_begin(&hs, proxies);
	while ((node = hash_scan_next(&hs))) {
		proxy_t *p = (proxy_t *)(node->hash_data);
		if (ppt->ip == p->ip) {
			hash_delete(proxies, node);
			break;
		}
	}

}

int main(int argc, char **argv) {
	char errbuf[PCAP_ERRBUF_SIZE];
	int *pthread_return;

	/* Initialization and Defaults */
	xboxen = hash_create(64, comparemac, NULL);
	proxies = hash_create(64, compareip, NULL);
	set_log_level(15);

	pthread_t pcapthread, proxythread;

	/* Argument Processing */
	if (argc > 1)
		pcapdev = argv[1];
	else
		pcapdev = pcap_lookupdev(errbuf);

	if (pcapdev == NULL) {
		debuglog(0, "No device specified or unable to find a device to open.");
		debuglog(0, "Error message: %s", errbuf);
		return 1;
	}

	pthread_create(&pcapthread, NULL, (void*)pcap, NULL);
	pthread_create(&proxythread, NULL, (void*)proxy, NULL);

	pthread_join(pcapthread, NULL);
	debuglog(0, "pcap thread exited unexpectedly");
	exit(1);
	pthread_join(proxythread, NULL);

	return 0;
}
