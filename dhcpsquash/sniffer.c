/*
 * BPF/libpcap stuff goes here.
 * Watch for specific DHCP and arp-related traffic
 *
 */

#include <sys/types.h>
#include <net/ethernet.h> /* for struct ether_header and such */
#include <netinet/in_systm.h>
#include <netinet/in.h> /* needed by netinet/ip.h */
#include <netinet/ip.h> /* for struct ip and such */
#include <netinet/udp.h> /* for struct udp and other udp-relatedness */
#include <pcap.h>
//include <libnet.h>

void packethandler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

void arp_packet(const u_char *packet, int offset);
void dhcp_packet(const u_char *packet, int offset);

int main(int argc, char **argv) {
	pcap_t *handle;
	struct bpf_program filter;
	char errbuf[PCAP_ERRBUF_SIZE];
	bpf_u_int32 mask, net;
	char *filterstring;
	char *pcapdev;

	pcapdev = "rl0"; /* pcap_lookupdev(errbuf) */

	pcap_lookupnet(pcapdev, &net, &mask, errbuf);
	handle = pcap_open_live(pcapdev, 1024, 1, 10, errbuf);

	if (handle == NULL) {
		printf("pcap_open_live: %s\n", errbuf);
		exit(1);
	}

	//filterstring = malloc(1024);
	//memset(filterstring, 0, 1024);

	if (argc == 2) {
		filterstring = *argv++;
	} else {
		filterstring = "((udp and (port 67 or port 68)) or (arp))";
	}

	if (-1 == pcap_compile(handle, &filter, filterstring, 1, net)) {
		fprintf(stderr, "pcap_compile: %s\n", pcap_geterr(handle));
		fprintf(stderr, "Filter string: %s\n", filterstring);
		exit(1);
	}

	pcap_setfilter(handle, &filter);
	pcap_loop(handle, -1, packethandler, NULL);

	/* Shouldn't get here */
	pcap_close(handle);

	return 0;
}

void packethandler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
	struct ether_header *eptr;
	char shost[ETHER_ADDR_LEN * 2 + 1],
		  dhost[ETHER_ADDR_LEN * 2 + 1];
	int offset = 0;

	eptr = (struct ether_header *) packet;

	if (ETHERTYPE_ARP == htons(eptr->ether_type)) {
		arp_packet(packet, offset);
		printf("(%04x)", htons(eptr->ether_type));
	} else if (ETHERTYPE_IP == htons(eptr->ether_type)) {
		struct ip *iptr;
		offset += sizeof(struct ether_header);
		iptr = (struct ip *)(packet + offset);
		if (IPPROTO_UDP == iptr->ip_p) {
			dhcp_packet(packet, offset);
		}
	} else {
		/* Unknown Packet */
		printf("UNKNOWN PACKET: ethertype %04x\n", eptr->ether_type);
	}

	printf(" %s => ", ether_ntoa((struct ether_addr *)eptr->ether_shost));
	printf("%s\n", ether_ntoa((struct ether_addr *)eptr->ether_dhost));
}

void dhcp_packet(const u_char *packet, int offset) {
	struct udphdr *uptr;
	struct ip *iptr;

	iptr = (struct ip *)(packet + offset);
	offset += sizeof(struct ip);
	uptr = (struct udphdr *)(packet + offset);

	printf("(udp[%d->%d] / ", htons(uptr->uh_sport), htons(uptr->uh_dport));
	printf("%s => ", inet_ntoa(iptr->ip_src));
	printf("%s)", inet_ntoa(iptr->ip_dst));
	printf("%d - ", htons(uptr->uh_ulen));
}

void arp_packet(const u_char *packet, int offset) {
	
}
