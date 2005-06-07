bin_PROGRAMS = proxy
proxy_SOURCES= main.c hash.c log.c hash.h log.h

CC = @CC@
AM_CFLAGS = -I. @LIBNET_CFLAGS@ -I@PCAP_PREFIX@
AM_LDFLAGS = @LIBNET_LDFLAGS@ -L@PCAP_PREFIX@
LIBS = @LIBS@ 
INSTALL = @INSTALL@
prefix = @prefix@
exec_prefix = @exec_prefix@
bindir = $(exec_prefix)/bin
libdir = $(prefix)/lib
infodir = $(prefix)/info
