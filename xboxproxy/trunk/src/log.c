/*
 * log.c
 *
 * $Id$
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "log.h"

FILE *logfile = NULL;
int loglevel = 0;

/* Set the log level */
void set_log_level(int level) {
	loglevel = level;
}

/* Log whatever if loglevel >= level */
void debuglog(int level, const char *format, ...) {
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

	//snprintf(string, 1024, "%s: %s\n", date_string, msg_string);
	snprintf(string, 1024, "%s\n", msg_string);
	fprintf(logfile, "%s", string);
	fflush(logfile);
}

void hexdump(char *buf, unsigned int len) {
	int c;
	char *p, *l, *dump, dumpbuf[40];

	l = &(buf[len-1]);
	dump = dumpbuf;
	putchar('\n');

	printf("[Hexdump]\n");
	for (p = buf; p <= l; p++)
	{   
		*(dump++) = (isprint((int)*p) ? *p : '.');
		putchar((c = (*p & 0xF0) >> 4) < 10 ? c + '0' : c + '7');
		putchar((c = *p & 0x0F) < 10 ? c + '0' : c + '7');
		if (!(((p-buf)+1) % 16) || (p == l))
		{   
			if(p == l) // pad last line
			{   
				while(((((p++)-buf)+1) % 16))
				{   
					putchar(' ');
					putchar(' ');
					putchar(' ');
				}
			}
			*dump = 0;
			putchar(' ');
			putchar(' ');
			puts(dumpbuf);
			dump = dumpbuf;
		}
		else
			putchar(' ');
	}
	putchar('\n');
}

