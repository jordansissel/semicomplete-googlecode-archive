#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(xboxproxy, 1.0, psionic@csh.rit.edu)
AM_INIT_AUTOMAKE(xboxproxy, 0.1)
AC_CONFIG_SRCDIR([main.c])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
LIBNET_LDFLAGS="`libnet-config --libs`"
LIBNET_CFLAGS="`libnet-config --defines` -D__GLIBC__"

# Checks for header files.

AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h limits.h netdb.h netinet/in.h stddef.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

AC_CHECK_HEADERS([netinet/if_ether.h net/ethernet.h], , , [#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
#AC_CHECK_FUNCS([gethostbyname inet_ntoa select socket strerror])

AC_CANONICAL_HOST

# pthreads...
echo "Host: $host"
case $host in
*solaris*)
	CFLAGS="$CFLAGS -D_POSIX_PTHREAD_SEMANTICS"
	LDFLAGS="$LDFLAGS -lpthread -lnsl -lsocket" ;;
*freebsd*)
	LDFLAGS="-pthread" ;;
esac

AC_CHECK_LIB(pthread,pthread_create,LD_FLAGS="$LDFLAGS -lpthread")

# Check for pcap
AC_ARG_WITH([pcap],
            AC_HELP_STRING([--with-pcap=DIR], 
									[search for pcap in DIR/include and DIR/lib]),
				[ PCAP_PREFIX=$withval ],
				[ PCAP_PREFIX=/usr/local ])

AC_CHECK_FILE([$PCAP_PREFIX/lib/libpcap.a],
				  [LDFLAGS="$LDFLAGS -L$PCAP_PREFIX/lib"
				  CFLAGS="$CFLAGS -I$PCAP_PREFIX/include" ],
				  AC_MSG_ERROR([
pcap not found in $PCAP_PREFIX. Specify path with --with-pcap=/path/to/pcap/dir
]))

AC_CHECK_HEADERS(pcap.h)
AC_CHECK_LIB(pcap, pcap_lib_version, [LDFLAGS="$LDFLAGS -lpcap"], 
				 [AC_MSG_ERROR([libpcap was not found])])


# Check for libnet
AC_ARG_WITH([libnet],
            AC_HELP_STRING([--with-libnet=DIR], 
									[search for libnet in DIR/include and DIR/lib]),
				[ LIBNET_PREFIX=$withval ],
				[ LIBNET_PREFIX=/usr/local ])


AC_CHECK_FILE([$LIBNET_PREFIX/lib/libnet.a],
				  [LDFLAGS="$LDFLAGS -L$LIBNET_PREFIX/lib"
				  CFLAGS="$CFLAGS -I$LIBNET_PREFIX/include" ],
				  AC_MSG_ERROR([
Libnet not found in $LIBNET_PREFIX. Specify path with --with-libnet
]))

AC_CHECK_LIB(net, libnet_write_link_layer, 
				 AC_DEFINE([HAVE_LIBNET_1_0], [], [libnet 1.0.x]),
				 AC_CHECK_LIB(net, libnet_adv_write_link,
								  AC_DEFINE([HAVE_LIBNET_1_1], [], [libnet 1.1.x]),
								  AC_MSG_ERROR([Libnet library not found in $LIBNET_PREFIX])))

AC_CHECK_TYPE(u_char, unsigned char)

AC_SUBST(LIBNET_PREFIX)
AC_SUBST(LIBNET_LDFLAGS)
AC_SUBST(LIBNET_CFLAGS)

AM_CONFIG_HEADER([config.h])
AC_OUTPUT(Makefile)
