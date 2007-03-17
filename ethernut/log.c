/*
 * log.c
 *
 * $Id$
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "log.h"

FILE *logfile = NULL;
int loglevel = 0;

/* Set the log level */
void set_log_level(int level) {
	loglevel = level;
}

/* Log whatever if loglevel >= level */
void log(int level, const char *format, ...) {
	va_list args;

	char string[1024];
	char msg_string[1024];
	char date_string[64];
	time_t ttime;

	if (level > loglevel)
		return;

	/* Check if logfile's been opened */
	if (logfile == NULL) {
		logfile = stdout;
	}
	ttime = time(NULL);
	ctime_r(&ttime, date_string);
	date_string[strlen(date_string) - 1] = 0;
	va_start(args, format);
	vsnprintf(msg_string, 1024, format, args);
	va_end(args);

	snprintf(string, 1024, "%s: %s\n", date_string, msg_string);
	fprintf(logfile, "%s", string);
	fflush(logfile);
}
