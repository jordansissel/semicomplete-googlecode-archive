/* 
 *
 * $Id$
 */

/* Various Includes {{{ */
#include <sys/types.h>
#include <sys/time.h>

#include "../config.h"

#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <string.h>

#ifdef __FreeBSD__
#include <netinet/in_systm.h>
#endif

#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#ifdef HAVE_NET_ETHERNET_H
#include <net/ethernet.h>
#define MACTYPE u_char
#define ETHERCONV(ether) (ether)
#endif

#ifdef HAVE_NETINET_IF_ETHER_H
#include <net/if.h>
#include <netinet/if_ether.h>
#define MACTYPE uchar_t
#define ETHERCONV(ether) (&((ether).ether_addr_octet))
#endif

#include <pthread.h>
#include <pcap.h>
#include <libnet.h>
/* End system includes }}} */

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN 6
#endif

#define SERVER_PORT 3434

#include "ugly.h"
#include "transport.h"

static int BIGSERVER;

#include "structures.h"
#include "log.h"
#include "hash.h"

/* xbox broadcasts to FF:FF:FF:FF:FF:FF */
static const char broadcastmac[ETHER_ADDR_LEN] = { 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

/* multicast mac address is 01:00:5E:XX:XX:XX */
static const char multicastmac[ETHER_ADDR_LEN] = {
	0x01, 0x00, 0x5E,
	0x00, 0x00, 0x00, /* Last 3 octets are unknown and don't matter */
};

hash_t *xboxen = NULL;
hash_t *proxies = NULL;
fd_set proxysocks;

char *progname;

char *pcapdev = NULL;
char *proxyserver = NULL;

static options_t options;

/* Callbacks */
int (*sendpacket)(proxy_t *ppt, char *packet, int packetlen, struct sockaddr *to, size_t tolen);
int (*recvpacket)(proxy_t *ppt, char *packet, int packetlen, struct sockaddr *to, size_t *tolen);

my_libnet_t *libnet;

void packet_handler(u_char *args, const struct pcap_pkthdr *head,
					  const u_char *packet);
void distribute_packet(proxy_t *ppt, char *packet, int pktlen);

void usage() {
	debuglog(0, 
"Usage: %s [-bxm] [-u] [-s <server>] [-i <dev>] [-d <debuglevel>] [-p <port>] [-h]",
				progname);
	debuglog(0, "-x              forward xbox system link packets");
	debuglog(0, "-b              forward broadcast traffic");
	debuglog(0, "-m              forward multicast packets");
	debuglog(0, "-u              use udp encapsulation instead of tcp (default)");
	debuglog(0, "-s <server>     specify another proxy to send packets to");
	debuglog(0, "-i <dev>        ethernet device to sniff packets on");
	debuglog(0, "-d <level>      specify debug level, (0-1000)");
	debuglog(0, "-p <port>       which port to send data on when talkin to other proxies");
	debuglog(0, "-f <bpf filter> an additional bpf filter string you wish to use");
	debuglog(0, "-h              this message!");
}

void proxy(void *args) {
	runserver();
}

int main(int argc, char **argv) {
	char errbuf[PCAP_ERRBUF_SIZE];
	char ch;

	progname = argv[0];

	/* Initialization and Defaults */
	init_storage();
	set_log_level(0);

	pthread_t pcapthread, proxythread;

	/* Argument Processing */
	while ((ch = getopt(argc, argv, "bxmus:i:d:h?p:f:")) != -1) {
		switch (ch) {
			case 'b':
				debuglog(10, "-b flag, enabling broadcast forwarding");
				options.forwardbroadcast = 1;
				break;
			case 'd':
				set_log_level(atoi(optarg));
				break;
			case 'f':
				options.user_filter = malloc(strlen(optarg));
				strcpy(options.user_filter, optarg);
				break;
			case 'i':
				options.pcapdev = malloc(strlen(optarg) + 1);
				strlcpy(options.pcapdev,optarg,strlen(optarg) + 1);
				debuglog(10, "-i flag, setting device to %s", options.pcapdev);
				break;
			case 'm':
				debuglog(10, "-m flag, enabling multicast forwarding");
				options.forwardmulticast = 1;
				break;
			case 'p':
				options.serverport = atoi(optarg);
				debuglog(10, "-p flag, setting proxy port to %d", options.serverport);
				break;
			case 's':
				options.proxyserver = malloc(strlen(optarg) + 1);
				strlcpy(options.proxyserver,optarg,strlen(optarg) + 1);
				debuglog(10, "-s flag, setting proxy server to %s", options.proxyserver);
				break;
			case 'u':
				debuglog(10, "-u flag, enabling udp");
				options.use_udp = 1;
				break;
			case 'x':
				debuglog(10, "-x flag, enabling xbox system link forwarding");
				options.forwardxbox = 1;
				break;
			case 'h':
			case '?':
			default:
				usage();
				exit(1);
				break;
		}
	}

	argc -= optind;
	argv += optind;

	if (pcapdev == NULL) 
		pcapdev = pcap_lookupdev(errbuf);

	if (pcapdev == NULL) {
		debuglog(0, "No device specified or unable to find a device to open.");
		debuglog(0, "Error message: %s", errbuf);
		return 1;
	}

	if (options.use_udp) {
		sendpacket = udp_sendpacket;
		recvpacket = udp_recvpacket;
	}

	pthread_create(&pcapthread, NULL, (void*)pcap, NULL);
	pthread_create(&proxythread, NULL, (void*)proxy, NULL);

	pthread_join(pcapthread, NULL);
	debuglog(0, "pcap thread exited unexpectedly");
	exit(1);
	pthread_join(proxythread, NULL);

	return 0;
}
