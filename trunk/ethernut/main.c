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
#include <signal.h>

#define _GNU_SOURCE
#include <string.h>

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

void cleanup(int sig) { 
	int i = 0;

	done = 1; 

	if (pthread_self() != mainthread)
		return; 

	log(0, "%s received - Cleaning up...", strsignal(sig));

	for (; i < threadcount; i++) {
		log(0, "Waiting on thread #%d to finish", i);
		pthread_join(threads[i], NULL);
	}

	exit(0);
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
