#ifndef _GROK_H_
#define _GROK_H_

#include <pcre.h>

typedef struct grok grok_t;
typedef struct grok_capture grok_capture_t;

typedef struct grok_pattern {
  const char *name;
  char *regexp;
} grok_pattern_t;

struct grok_capture {
  const char *name;
  const char *pattern;
  int id;
  int pcre_capture_number;

  int (*predicate_func)(grok_t *, grok_capture_t *, const char *, int, int);
  void *extra;
  const char *predicate; /* XXX: Delete this */
};

struct grok {
  /* tree of grok_pattern objects */
  void *patterns;
  
  /* These are initialized when grok_compile is called */
  pcre *re;
  const char *pattern;
  char *full_pattern;
  int *pcre_capture_vector;
  int pcre_num_captures;
  
  /* Data storage for named-capture (grok capture) information */
  void *captures_by_id;
  void *captures_by_name;
  void *captures_by_capture_number;
  int max_capture_num;
  
  /* PCRE pattern compilation errors */
  const char *pcre_errptr;
  int pcre_erroffset;
};

typedef struct grok_match {
  grok_t *grok;
  const char *subject;
} grok_match_t;

#include "grokre.h"

#endif /* ifndef _GROK_H_ */
