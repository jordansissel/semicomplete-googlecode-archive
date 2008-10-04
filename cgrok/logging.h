#ifndef _LOGGING_H_
#define _LOGGING_H_

#include "grok.h"

#define LOG_PREDICATE (1 << 0)
#define LOG_COMPILE (1 << 1)
#define LOG_EXEC (1 << 2)
#define LOG_REGEXPAND (1 << 3)
#define LOG_PATTERNS (1 << 4)

#define LOG_ALL (LOG_PREDICATE | LOG_COMPILE | LOG_EXEC \
                 | LOG_REGEXPAND | LOG_PATTERNS)

void grok_log(grok_t *grok, int level, const char *format, ...);

#endif /* _LOGGING_H_ */
