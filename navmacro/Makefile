CFLAGS=-g

navmacro: navmacro.o
	gcc $(CFLAGS) `pkg-config --libs x11 gtk+-2.0` navmacro.o -o $@

navmacro.o:
	gcc $(CFLAGS) `pkg-config --cflags x11 gtk+-2.0` -c navmacro.c

querytree: querytree.o
	gcc $(CFLAGS) `pkg-config --libs x11` querytree.o -o $@

querytree.o:
	gcc $(CFLAGS) `pkg-config --cflags x11` -c querytree.c

xdo: xdo.o
	gcc $(CFLAGS) `pkg-config --libs x11 xtst` xdo.o -o $@

xdo.o:
	gcc $(CFLAGS) `pkg-config --cflags x11 xtst` -c xdo.c
