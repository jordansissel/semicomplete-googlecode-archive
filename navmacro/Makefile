
navmacro: navmacro.o
	gcc `pkg-config --libs x11 gtk+-2.0` navmacro.o -o $@

navmacro.o:
	gcc `pkg-config --cflags x11 gtk+-2.0` -c navmacro.c

querytree: querytree.o
	gcc `pkg-config --libs x11` querytree.o -o $@

querytree.o:
	gcc `pkg-config --cflags x11` -c querytree.c

xdo: xdo.o
	gcc `pkg-config --libs x11 xtst` xdo.o -o $@

xdo.o:
	gcc `pkg-config --cflags x11 xtst` -c xdo.c
