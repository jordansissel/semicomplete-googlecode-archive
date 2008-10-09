#include <stdio.h>
#include <stdarg.h>
#include "grok.h"

#ifndef NOLOGGING
inline void grok_log(grok_t *grok, int level, const char *format, ...) {
  va_list args;
  FILE *out;

  out = stderr;
  if (!(grok->logmask & level))
    return;

  va_start(args, format);
  char *prefix;
  switch (level) {
    case LOG_PREDICATE: prefix = "[predicate] "; break;
    case LOG_COMPILE: prefix = "[compile] "; break;
    case LOG_EXEC: prefix = "[exec] "; break;
    case LOG_REGEXPAND: prefix = "[regexpand] "; break;
    case LOG_PATTERNS: prefix = "[patterns] "; break;
    default: prefix = "[unknown] ";
  }
  fprintf(out, "% *s%s", grok->logdepth * 3, "", prefix);
  vfprintf(out, format, args);
  fprintf(out, "\n");
  va_end(args);
}
#endif
