/*
 * log.h
 *
 * $Id$
 */

#include <stdio.h>

/* Log level defaults to 0 */
int loglevel = 0;

FILE *log_file = stdout;

/* Set the log level */
void set_log_level(int level);

/* Log.
 * Will log whatever if loglevel >= level
void log(int level, const char *format, ...);
