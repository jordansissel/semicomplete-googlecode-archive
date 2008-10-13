#include <stdio.h>
#include <stdarg.h>
#include "grok.h"

#ifndef NOLOGGING
inline void _grok_log(int level, int indent, const char *format, ...) {
  va_list args;
  FILE *out;

  out = stderr;

  va_start(args, format);
  char *prefix;
  switch (level) {
    case LOG_PREDICATE: prefix = "[predicate] "; break;
    case LOG_COMPILE: prefix = "[compile] "; break;
    case LOG_EXEC: prefix = "[exec] "; break;
    case LOG_REGEXPAND: prefix = "[regexpand] "; break;
    case LOG_PATTERNS: prefix = "[patterns] "; break;
    case LOG_MATCH: prefix = "[match] "; break;
    case LOG_CAPTURE: prefix = "[capture] "; break;
    default: prefix = "[unknown] ";
  }
  fprintf(out, "[%d] % *s%s", getpid(), indent * 3, "", prefix);
  vfprintf(out, format, args);
  fprintf(out, "\n");
  va_end(args);
}
#endif
