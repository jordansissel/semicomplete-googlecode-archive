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
#include <rrd.h>

#include <signal.h>
#include <unistd.h>

#ifdef __FreeBSD__
#include <netinet/in_systm.h>
#endif

#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

/* How long do we wait before reporting data? (in seconds) */
#define WAIT_SECONDS 11

/* CSH has a /23, that means 512 addresses. */
#define CSH_NET_SIZE 512

static char     filter_app[] = "(tcp or udp) and not (multicast or dst net 129.21.0.0/16 or dst net 192.168.0.0/16 or dst net 10.0.0.0/8)";

static int cshnet[CSH_NET_SIZE];

void
sigalarm(int sig) {
	if (sig == SIGALRM) {
		int i;

		for (i = 1; i < CSH_NET_SIZE; i++) {
			int *this = (cshnet + i);
			int argc = 0;
			char *argv[10];
			short c, d;
			char file[50], 
				  data[50];

			c = (i >> 8) + 60;
			d = (i & 0xff);

			if ((d > 0) && (d < 255) && *this > 0) {

				sprintf(file, "/mnt/129.21.%d.%d.rrd\0", c, d);
				sprintf(data, "N:%d\0", *this);

				argv[argc++] = "update";
				argv[argc++] = file;
				argv[argc++] = data;
				argv[argc] = NULL;


				rrd_clear_error();
				rrd_update(argc, argv);
				if (rrd_test_error()) {
					fprintf(stderr, "RRD ERROR: %s\n", rrd_get_error());
				}

				if (*this > 0) {
					printf("129.21.%d.%d: %d\n", c, d, *this);
					*this = 0;
				}
			}

		}

		alarm(WAIT_SECONDS);
	}
}

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

	if (ether_type = ETHERTYPE_IP) {
		u_int offset;
		int c, d;

		ip = (struct ip*)(packet + sizeof(struct ether_header));
		length -= sizeof(struct ether_header);

		if (length < sizeof(struct ip)) {
			printf("truncated packet, ignoring, length: %d < %d\n", 
			       length, sizeof(struct ip));
			return;
		}

		offset = ntohs(ip->ip_off);

		/* Is this the first fragment? */
		if ((offset & 0x1fff) == 0) {
			/*
			printf("IP Packet\n");
			printf("\tSRCIP: %s\n", inet_ntoa(ip->ip_src));
			printf("\tDSTIP: %s\n", inet_ntoa(ip->ip_dst));
			printf("\tLEN: 0x%x\n", ip->ip_len);
			*/

			/* 0x00010000 - 129.21.60.0/23, possible C block IPs can be
			 * 60 or 61, either of which end in either 0 or 1, so anding
			 * 0x00010000 tells us which block we're on, so we don't have to
			 * subtract 60.
			 * 0x0001ffff being a /23 subnet, shortcuts == fasterness (jordan)
			 */
			c = (ip->ip_src.s_addr & 0x00010000) >> 8;
			d = ip->ip_src.s_addr >> 24;

			cshnet[c + d] += ntohs(ip->ip_len);
		}
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
	const u_char   *packet;
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

	handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);

	if (handle == NULL) {
		fprintf(stderr, "Error trying to open %s. %s\n", dev, errbuf);
		exit(1);
	} 
	printf("Opened %s\n", dev);

	pcap_compile(handle, &filter, filter_app, 0, net);
	pcap_setfilter(handle, &filter);

	pcap_loop(handle, -1, packety, NULL);

	pcap_close(handle);

	return 0;
}
