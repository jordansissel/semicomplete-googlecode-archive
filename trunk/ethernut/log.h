/*
 * log.h
 *
 * $Id$
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>

/* Set the log level */
void set_log_level(int level);

/* Will log whatever if loglevel >= level */
void log(int level, const char *format, ...);

#endif
