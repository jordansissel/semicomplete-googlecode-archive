/*
 * udp.h
 *
 * Function headers for UDP transport functions used
 * to send and receive packets
 *
 * $Id$
 */

#ifndef UDP_H
#define UDP_H

#include "../config.h"

#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>

#ifdef __FreeBSD__
#include <netinet/in_systm.h>
#endif

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


#include "structures.h"

int udp_sendpacket(proxy_t *ppt, char *packet, int packetlen, struct sockaddr *to, size_t tolen);
int udp_recvpacket(proxy_t *ppt, char *packet, int packetlen, struct sockaddr *from, size_t *fromlen);
int udp_connect(proxy_t *ppt);
void udp_runserver();

#endif /* UDP_H */
