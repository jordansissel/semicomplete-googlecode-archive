/*
 * CSH Bandwidth Monitor
 *
 * Looks for outbound IP packets and keeps track of per-ip bandwidth usage
 *
 */

#include <sys/types.h>
#include <stdio.h>
#include <pcap.h>

#ifdef __FreeBSD__
#include <netinet/in_systm.h>
#endif

#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

static char     filter_app[] = "ip and not multicast and not dst net 129.21.0.0/16 and not dst net 192.168.0.0/16 and not dst net 10.0.0.0/8";

static cshnet[512]

void
packety(u_char *args, const struct pcap_pkthdr *header,
	const u_char *packet)
{
	const struct ip  *ip;
	struct ether_header *eptr;
	u_short         ether_type;
	u_int length = header->len;

	eptr = (struct ether_header *) packet;
	ether_type = ntohs(eptr->ether_type);

	ip = (struct ip*)(packet + sizeof(struct ether_header));
	length -= sizeof(struct ether_header);

	if (ether_type = ETHERTYPE_IP) {
		printf("IP Packet\n");
		printf("\tSRCIP: %s\n", inet_ntoa(ip->ip_src));
		printf("\tDSTIP: %s\n", inet_ntoa(ip->ip_dst));
		printf("\tSIZE: %d\n", length);
	}
}

int
main(int argc, char **argv)
{

	struct bpf_program filter;
	pcap_t         *handle;
	bpf_u_int32     mask;
	bpf_u_int32     net;
	struct pcap_pkthdr header;
	const u_char * packet;
	char           *dev, errbuf[PCAP_ERRBUF_SIZE];

	if (argc > 1)
		dev = argv[1];
	else
		dev = pcap_lookupdev(errbuf);

	if (dev == NULL) {
		fprintf(stderr, "No device specified or unable to find a device to open.\n%s\n", errbuf);
		return 1;
	}
	printf("Using device: %s\n", dev);

	pcap_lookupnet(dev, &net, &mask, errbuf);

	handle = pcap_open_live(dev, 100, 1, 1000, errbuf);
	printf("Opened %s\n", dev);

	pcap_compile(handle, &filter, filter_app, 0, net);
	pcap_setfilter(handle, &filter);

	pcap_loop(handle, -1, packety, NULL);

	pcap_close(handle);

	return 0;
}
