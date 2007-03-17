/*
 * ugly.h is meant to help clean up the rest of the source code.
 *
 * It contains macros and such to help cross-platformisms along, and
 * make the code easier to read
 *
 * $Id$
 */

#ifndef UGLY_H
#define UGLY_H

#include <libnet.h>

#include "../config.h"

#ifdef HAVE_LIBNET_1_0
typedef struct libnet_link_int my_libnet_t;
#else 
#  ifdef HAVE_LIBNET_1_1
typedef libnet_t my_libnet_t;
#  else 
#     error LIBNET LIBRARY MISSING
#  endif 
#endif

my_libnet_t *my_libnet_init(char *pcapdev, char *errbuf);
int my_libnet_write_link_layer(my_libnet_t *libnet, char *pcapdev, char *packet, int pktlen);

#endif
