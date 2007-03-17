INCS=-I/usr/local/include
LIBS=-L/usr/lib -lpcap -L/usr/local/lib -lrrd
BUILDY=gcc -g ${INCS} ${LIBS} -o bandmon

WATCHER=-DWATCHER
RRD=-DRRD_UPDATES
all:
	${BUILDY} ${RRD} bandmon.c

watcher:
	${BUILDY} ${WATCHER} bandmon.c

