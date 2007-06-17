
navmacro: navmacro.o
	gcc `pkg-config --libs x11 gtk+-2.0` navmacro.o -o $@

navmacro.o:
	gcc `pkg-config --cflags x11 gtk+-2.0` -c navmacro.c



