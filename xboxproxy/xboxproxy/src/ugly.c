/*
 * ugly.h is meant to help clean up the rest of the source code.
 *
 * It contains macros and such to help cross-platformisms along, and
 * make the code easier to read
 *
 * $Id$
 */

#include <libnet.h>
#include "ugly.h"
#include "log.h"

/*
 * my_libnet_init
 */
my_libnet_t *my_libnet_init(char *pcapdev, char *errbuf) {
#ifdef HAVE_LIBNET_1_0 
	return libnet_open_link_interface(pcapdev, errbuf);
#else 
#	ifdef HAVE_LIBNET_1_1 
	return libnet_init(LIBNET_LINK_ADV, pcapdev, errbuf); 
#	endif
#endif
}

/* 
 * my_libnet_write_link_layer
 */
int my_libnet_write_link_layer(my_libnet_t *libnet, char *pcapdev, char *packet, int pktlen) {
#ifdef HAVE_LIBNET_1_0
	return libnet_write_link_layer(libnet, pcapdev, packet, pktlen);
#else
#  ifdef HAVE_LIBNET_1_1
	return libnet_adv_write_link(libnet, packet, pktlen);
#  endif
#endif
}
