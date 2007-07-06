CFLAGS=-g

all: runbox

clean:
	rm -f *.o

runbox: runbox.o
	gcc $(CFLAGS) `pkg-config --libs x11 xtst gtk+-2.0` runbox.o -o $@

runbox.o:
	gcc $(CFLAGS) `pkg-config --cflags x11 gtk+-2.0` -c runbox.c

querytree: querytree.o
	gcc $(CFLAGS) `pkg-config --libs x11` querytree.o -o $@

package:
	NAME=runbox-`date +%Y%m%d`; \
	mkdir $${NAME}; \
	rsync --exclude .svn -av *.c *.h README Makefile examples $${NAME}/; \
	tar -zcf $${NAME}.tar.gz $${NAME}/; \
	rm -rf $${NAME}/

