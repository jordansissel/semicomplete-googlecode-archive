/*
 * CSH Bandwidth Monitor
 *
 * Looks for outbound IP packets and keeps track of per-ip bandwidth usage
 *
 */

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <pcap.h>

#include <signal.h>
#include <unistd.h>

#ifdef __FreeBSD__
#include <netinet/in_systm.h>
#endif

#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

/* How long do we wait before reporting data? (in seconds) */
#define WAIT_SECONDS 60

/* CSH has a /23, that means 512 addresses. */
#define CSH_NET_SIZE 512

static char     filter_app[] = "ip and not multicast and not dst net 129.21.0.0/16 and not dst net 192.168.0.0/16 and not dst net 10.0.0.0/8";

static int cshnet[CSH_NET_SIZE];
static short lock;

void
sigalarm(int sig) {
	if (sig == SIGALRM) {
		int i;
		while (lock);

		lock = 1;

		for (i = 0; i < CSH_NET_SIZE; i++) {
			int *this = (cshnet + i);

			if (*this > 0) {
				printf("129.21.%d.%d: %d\n", (i >> 8) + 60, (i & 0xff) + 1, *this);
				*this = 0;
			}
		}
		lock = 0;
	}
	alarm(WAIT_SECONDS);
}

void
packety(u_char *args, const struct pcap_pkthdr *header,
	const u_char *packet)
{

	const struct ip  *ip;
	struct ether_header *eptr;
	u_short         ether_type;
	u_int length = header->len;

	while (lock) sleep(1);
	lock = 1;

	eptr = (struct ether_header *) packet;
	ether_type = ntohs(eptr->ether_type);

	ip = (struct ip*)(packet + sizeof(struct ether_header));
	length -= sizeof(struct ether_header);

	if (ether_type = ETHERTYPE_IP) {
		int c, d;

		/*
		printf("IP Packet\n");
		printf("\tSRCIP: %s\n", inet_ntoa(ip->ip_src));
		printf("\tDSTIP: %s\n", inet_ntoa(ip->ip_dst));
		printf("\tSIZE: %d\n", length);
		*/

		/* 0x00010000 - 129.21.60.0/23, possible C block IPs can be
		 * 60 or 61, either of which end in either 0 or 1, so anding
		 * 0x00010000 tells us which block we're on, so we don't have to
		 * subtract 60.
		 * 0x0001ffff being a /23 subnet, shortcuts == fasterness (jordan)
		 */
		c = (ip->ip_src.s_addr & 0x00010000) >> 16;
		d = ip->ip_src.s_addr >> 24;

		cshnet[c*255 + d] += length;
	}

	lock = 0;
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

	signal(SIGALRM, sigalarm);
	alarm(WAIT_SECONDS);

	pcap_lookupnet(dev, &net, &mask, errbuf);

	handle = pcap_open_live(dev, 100, 1, 1000, errbuf);
	printf("Opened %s\n", dev);

	pcap_compile(handle, &filter, filter_app, 0, net);
	pcap_setfilter(handle, &filter);

	pcap_loop(handle, -1, packety, NULL);

	pcap_close(handle);

	return 0;
}
