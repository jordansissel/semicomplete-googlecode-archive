/* 
 *
 * $Id$
 */

#include <sys/types.h>
#include <sys/time.h>

#include <pcap.h>

#include <unistd.h>

#ifdef __FreeBSD__
#include <netinet/in_systm.h>
#endif

#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN 6
#endif

#include "log.h"
#include "hash.h"

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
} xbox_t;

static hash_t *xboxen = NULL;

void addxbox(u_char *macaddr, int proxyip);
void packet_handler(u_char *args, const struct pcap_pkthdr *head,
						  const u_char *packet);

int comparemac(const void *k1, const void *k2) {
	return memcmp(k1, k2, ETHER_ADDR_LEN);
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
		debuglog(1, "BROADCAST");
		//for all known packety things
		//sendpacket...
	}

}

void addxbox(u_char *macaddr, int proxyip) {
	xbox_t *newbox = NULL;
	hnode_t *box = NULL;

	box = hash_lookup(xboxen, macaddr);
	if (box == NULL) {
		debuglog(1, "New xbox found: %s", ether_ntoa((struct ether_addr *)macaddr));
		newbox = malloc(sizeof(xbox_t));
		hash_alloc_insert(xboxen, macaddr, newbox);
	}
}

int main(int argc, char **argv) {
	pcap_t *handle;
	struct bpf_program filter;
	char errbuf[PCAP_ERRBUF_SIZE];
	char *dev = NULL;
	bpf_u_int32	mask, net;

	set_log_level(15);

	/* Initialization */
	xboxen = hash_create(64, comparemac, NULL);

	/* Argument Processing */
	if (argc > 1)
		dev = argv[1];
	else
		dev = pcap_lookupdev(errbuf);

	if (dev == NULL) {
		debuglog(0, "No device specified or unable to find a device to open.");
		debuglog(0, "Error message: %s", errbuf);
		return 1;
	}

	debuglog(1, "Using device %s", dev);

	pcap_lookupnet(dev, &net, &mask, errbuf);

	handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);

	if (handle == NULL) {
		debuglog(0, "Error trying to open %s: %s", dev, errbuf);
		return 1;
	}

	debuglog(2, "Opened %s\n", dev);

	pcap_compile(handle, &filter, filter_app, 0, net);
	pcap_setfilter(handle, &filter);

	pcap_loop(handle, -1, packet_handler, NULL);

	pcap_close(handle);

	return 0;
}
