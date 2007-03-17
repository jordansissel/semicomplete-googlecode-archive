/*
 * transport.h!
 *
 * $Id$
 */

#ifndef TRANSPORT_H
#define TRANSPORT_H

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

//include "tcp.h"
#include "udp.h"

#endif /* TRANSPORT_H */
