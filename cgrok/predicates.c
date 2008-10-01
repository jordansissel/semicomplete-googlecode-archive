
#include <stdio.h>
#include <string.h>

#include "predicates.h"

static pcre *regexp_predicate_op = NULL;
#define REGEXP_PREDICATE_RE "(?:\\s*([!=])~\\s*/([^\\/]+|(?:\\/)+)*/)"

static void grok_predicate_regexp_global_init(void);


int grok_predicate_regexp(grok_t *grok, grok_capture_t *gct,
                          const char *subject, int start, int end) {
  grok_predicate_regexp_t *gprt = (grok_predicate_regexp_t *)gct->extra;
  int ret;

  ret = pcre_exec(gprt->re, NULL, subject + start, end - start, 0, 0, NULL, 0);
  //printf("%d: '%.*s'\n", ret, (end - start), subject + start);


  /* match found if ret > 0 */
  return (ret < 0) ^ (gprt->negative_match);
}

int grok_predicate_regexp_init(grok_t *grok, grok_capture_t *gct,
                               const char *args) {
  #define REGEXP_OVEC_SIZE 6
  int capture_vector[REGEXP_OVEC_SIZE * 3];
  int ret; 

  grok_predicate_regexp_global_init();
  ret = pcre_exec(regexp_predicate_op, NULL, args, strlen(args), 0, 0,
                  capture_vector, REGEXP_OVEC_SIZE * 3);
  if (ret < 0) {
    fprintf(stderr, "An error occured in grok_predicate_regexp_init.\n");
    fprintf(stderr, "Args: %s\n", args);
    return;
  }

  int start, end;
  const char *errptr;
  int erroffset;
  grok_predicate_regexp_t *gprt;
  start = capture_vector[4]; /* capture #2 */
  end = capture_vector[5];

  gprt = calloc(1, sizeof(grok_predicate_regexp_t));
  gprt->pattern = calloc(1, end - start + 1);
  strncpy(gprt->pattern, args + start, end - start);
  gprt->re = pcre_compile(gprt->pattern, 0, &errptr, &erroffset, NULL);
  gprt->negative_match = (args[capture_vector[2]] == '!');

  if (gprt->re == NULL) {
    fprintf(stderr, 
            "An error occured while compiling the predicate for %s:\n",
            gct->name);
    fprintf(stderr, "Error at pos %d: %s\n", erroffset, errptr);
    return;
  }

  fprintf(stderr, "Compiled regex for '%s': '%s'\n", gct->name, gprt->pattern);
  gct->extra = gprt;
}

static void grok_predicate_regexp_global_init(void) {
  if (regexp_predicate_op == NULL) {
    int erroffset = -1;
    const char *errp;
    regexp_predicate_op = pcre_compile(REGEXP_PREDICATE_RE, 0, 
                                       &errp, &erroffset, NULL);
    if (regexp_predicate_op == NULL) {
      fprintf(stderr, "Internal error (compiling predicate regexp op): %s\n",
              errp);
    }
  }
}
