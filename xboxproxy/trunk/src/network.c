/*
 * network.c
 *
 * $Id$
 */

#include "network.h"

void network_dump_packet(proxy_t *ppt, char *packet, int pktlen) {
	struct ether_header *eptr;
	u_short ether_type;
	int bytes = 0;

	eptr = (struct ether_header *)packet;
	ether_type = ntohs(eptr->ether_type);

	debuglog(30, "----- REMOTE PACKET From: %s",
				ether_ntoa((struct ether_addr *)ETHERCONV(eptr->ether_shost)));
	debuglog(30, "----- REMOTE PACKET To: %s",
				ether_ntoa((struct ether_addr *)ETHERCONV(eptr->ether_dhost)));

	bytes = my_libnet_write_link_layer(libnet,pcapdev,packet,pktlen);

	if (bytes < 0) {
		debuglog(0, "FATAL ERROR WRITING RAW PACKET TO DEVICE");
	} else {
		debuglog(4, "Packet dumped on local net (%s), bytes vs length = %d vs %d",
					pcapdev, bytes, pktlen);
	}

	if (get_log_level() > 10)
		hexdump(packet, pktlen);
}
/* 
 *  * Packet handling routine.
 *   */
void packet_handler(u_char *args, const struct pcap_pkthdr *head,
						  const u_char *packet) {
	struct ether_header *eptr;

	eptr = (struct ether_header *) packet;

	/* Try adding this xbox to the list */
	//debuglog(1, "Adding xbox in packet_handler");
	addxbox((MACTYPE *) ETHERCONV(eptr->ether_shost), 0);

	/* If this packet is sent to our local net, ignore it... */
	if (hash_lookup(xboxen, ETHERCONV(eptr->ether_shost)) != NULL) {
		debuglog(15, "Packet!");
		debuglog(3, "From: %s", ether_ntoa((struct ether_addr *)ETHERCONV(eptr->ether_shost)));
		debuglog(3, "To: %s", ether_ntoa((struct ether_addr *)ETHERCONV(eptr->ether_dhost)));
	}

	/*
	 * 	 * If this is broadcast or multicast, send it to all known proxies
	 * 	 	 */
	if ((memcmp(ETHERCONV(eptr->ether_dhost), broadcastmac, ETHER_ADDR_LEN) == 0) ||
		 (options.forwardmulticast && (memcmp(ETHERCONV(eptr->ether_dhost), multicastmac, 3) == 0))) {
		hscan_t hs;
		hnode_t *node;

		debuglog(11, "BROADCAST");
		hash_scan_begin(&hs, proxies);
		while ((node = hash_scan_next(&hs))) {
			proxy_t *p = (proxy_t *)(node->hash_data);

			/* Make sure the source addr of this packet is not getting
			 * 			 * sent this packet. Do not send! */
			if ((hash_lookup(p->xboxen, ETHERCONV(eptr->ether_shost)) != NULL))
				continue;

			debuglog(3, "Sending ethernet packet to %s:%d [Length: %d]", inet_ntoa(p->addr), htons(p->port), head->caplen);

			if (options.use_udp) {
				struct sockaddr_in to;
				to.sin_addr = p->addr;
				//to.sin_port = htons(serverport);
				to.sin_port = p->port;
				to.sin_family = PF_INET;

				sendto(p->fd, packet, head->caplen, 0, (struct sockaddr *)&to, sizeof(struct sockaddr));
			} else {
				/* TCP */
			}
		}
	} else {
		/* This packet is for a specific mac */
		proxy_t *p;
		hnode_t *box;

		box = hash_lookup(xboxen, ETHERCONV(eptr->ether_dhost));
		if (box == NULL) {
			debuglog(3, "Unknown destination %s, hasn't been seen yet.", 
						ether_ntoa((struct ether_addr *)ETHERCONV(eptr->ether_dhost)));
			return;
		}

		debuglog(3, "Found packet destined for %s! (len: %d)", 
					ether_ntoa((struct ether_addr *)ETHERCONV(eptr->ether_dhost)),
					head->caplen);

		p = ((xbox_t *)(box->hash_data))->proxy;

		if (p == NULL) {
			debuglog(3, "Packet is destined for this net, ignore it");
			return;
		}

		debuglog(3, "Proxy is: %s", (p == NULL ? "Local" : inet_ntoa(p->addr)));

		if (options.use_udp) {
			struct sockaddr_in to;
			to.sin_addr = p->addr;
			//to.sin_port = htons(serverport);
			to.sin_port = p->port;
			to.sin_family = PF_INET;

			sendto(p->fd, packet, head->caplen, 0, (struct sockaddr *)&to, sizeof(struct sockaddr));
		} else {
			/* TCP */
		}
	}
}

void network_run(void *args) {
	pcap_t *handle;
	struct bpf_program filter;
	char errbuf[PCAP_ERRBUF_SIZE];
	bpf_u_int32	mask, net;
	char *bpf_filter;

	char netstring[17];
	char maskstring[17];

	pcap_lookupnet(pcapdev, &net, &mask, errbuf);

	#define GETBYTE(data,i) (*(((unsigned char *)&data) + i))
	sprintf(netstring, "%u.%u.%u.%u", GETBYTE(net, 0), GETBYTE(net,1),
			  GETBYTE(net,2), GETBYTE(net, 3));
	sprintf(maskstring, "%u.%u.%u.%u", GETBYTE(mask, 0), GETBYTE(mask,1),
			  GETBYTE(mask,2), GETBYTE(mask, 3));

	debuglog(0, "Network: %s / %s", netstring, maskstring);

	/* A generous cap length */
	handle = pcap_open_live(pcapdev, 2048, 1, 10, errbuf);

	if (handle == NULL) {
		debuglog(0, "Error trying to open %s: %s", pcapdev, errbuf);
		//return 1;
		pthread_exit(NULL);
	}

	libnet = my_libnet_init(pcapdev, errbuf);
	if (libnet == NULL) {
		debuglog(0, "libnet_init() failed: %s", errbuf);
		pthread_exit(NULL);
	}

	debuglog(2, "Opened %s, listening for xbox traffic", pcapdev);

	bpf_filter = malloc(1024);
	memset(bpf_filter, 0, 1024);

	if (options.user_filter != NULL) {
		sprintf(bpf_filter, "(%s)", options.user_filter);
		if (options.forwardmulticast || options.forwardbroadcast || options.forwardxbox)
			sprintf(bpf_filter + strlen(bpf_filter), " and (");
	}

	/* Build the pcap bpf filter string */
	if (options.forwardmulticast) {
		sprintf(bpf_filter + strlen(bpf_filter), "(src net %s mask %s and (multicast))", netstring, maskstring);
		if (options.forwardbroadcast || options.forwardxbox)
			sprintf(bpf_filter + strlen(bpf_filter), " or ");
	}

	/* XXX: Should this match any broadcast? Ether broadcast? or just ip broadcast? */
	if (options.forwardbroadcast) {
		sprintf(bpf_filter + strlen(bpf_filter), "(ip broadcast)");
		if (options.forwardxbox) 
			sprintf(bpf_filter + strlen(bpf_filter), " or ");
	}

	if (options.forwardxbox)
		sprintf(bpf_filter + strlen(bpf_filter), "(host 0.0.0.1)");

	if (options.user_filter != NULL)
		sprintf(bpf_filter + strlen(bpf_filter), ")");

	debuglog(0, "pcap filter: %s", bpf_filter);

	if (-1 == pcap_compile(handle, &filter, bpf_filter, 1, net)) {
		debuglog(0, "%s: %s", progname, pcap_geterr(handle));
		if (options.user_filter != NULL) {
			debuglog(0, "%s: You specified an additional packet filter, it probably has an error", 
						progname);
			debuglog(0, "%s: The filter is '%s'", progname, options.user_filter);
			debuglog(5, "%s: Generated filter: %s", progname, bpf_filter);
		}
		exit(-1);
	}


	pcap_setfilter(handle, &filter);
	pcap_loop(handle, -1, packet_handler, NULL);

	pcap_close(handle);

	pthread_exit(NULL);
}
