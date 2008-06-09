#ifndef _PREDICATES_H_
#define _PREDICATES_H_

#include <pcre.h>
#include "grok.h"

/* Regular Expression Predicate
 * Activate with operator '=~'
 */

typedef struct grok_predicate_regexp {
  pcre *re;
  char *pattern;
} grok_predicate_regexp_t;

int grok_predicate_regexp(grok_t *grok, grok_capture_t *gct,
                          const char *subject, int start, int end);

int grok_predicate_regexp_init(grok_t *grok, grok_capture_t *gct,
                               const char *args);


#endif /* _PREDICATES_H_ */
