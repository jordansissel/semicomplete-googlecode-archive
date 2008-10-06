#ifndef _GROK_H_
#define _GROK_H_

#include <pcre.h>
#include <db.h>

typedef struct grok grok_t;

typedef struct grok_pattern {
  const char *name;
  char *regexp;
} grok_pattern_t;

struct grok {
  DB *patterns;
  
  /* These are initialized when grok_compile is called */
  pcre *re;
  const char *pattern;
  char *full_pattern;
  int *pcre_capture_vector;
  int pcre_num_captures;
  
  /* Data storage for named-capture (grok capture) information */
  DB *captures_by_id;
  DB *captures_by_name;
  DB *captures_by_capture_number;
  int max_capture_num;
  
  /* PCRE pattern compilation errors */
  const char *pcre_errptr;
  int pcre_erroffset;

  unsigned int logmask;
};

typedef struct grok_match {
  grok_t *grok;
  const char *subject;
} grok_match_t;

#include "grokre.h"
#include "logging.h"

#ifndef GROK_TEST_NO_PATTERNS
#include "grok_pattern.h"
#endif

#ifndef GROK_TEST_NO_CAPTURE
#include "grok_capture.h"
#endif
#endif /* ifndef _GROK_H_ */
