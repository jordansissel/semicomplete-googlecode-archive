/*
 * main.c
 *
 * Ethernut main thread
 * 
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "busmaster.h"
#include "network.h"
#include "i2c.h"
#include "log.h"

/* from busmaster.c */
extern busmaster_t *busmaster;
int done = 0;

int threadcount;
pthread_t *threads;
pthread_t mainthread;

void addthread(pthread_t thread) {
	threadcount++;
	threads = realloc(threads, sizeof(pthread_t) * threadcount);
	threads[threadcount - 1] = thread;
}

int main(int argc, char **argv) {

	mainthread = pthread_self();

	set_log_level(100);

	busmaster_init();
	network_init();
	i2c_init();

	//signal(SIGINT, cleanup);
	//signal(SIGTERM, cleanup);
	//signal(SIGHUP, cleanup);

	for (;;) {
		//log(0, "main says hi");
		sleep(10);
	}
	return 0;
}
