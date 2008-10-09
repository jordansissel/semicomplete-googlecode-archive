#ifndef _GROK_MATCH_H_
#define _GROK_MATCH_H_

#include "grok_capture.h"

typedef struct grok_match {
  grok_t *grok;
  const char *subject;
} grok_match_t;

int grok_match_get_named_capture(const grok_match_t *gm,
                                 const char * const name, grok_capture *gct);
int grok_match_get_named_substring(const grok_match_t *gm, const char *name,
                                   const char **substr, int *len);

#endif /*  _GROK_MATCH_H_ */
