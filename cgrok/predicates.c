
#include <stdio.h>
#include <string.h>

#include "predicates.h"

static pcre *regexp_predicate_op = NULL;
#define REGEXP_PREDICATE_RE "(?:\\s*([!=])~\\s*/([^\\/]+|(?:\\/)+)*/)"

static void grok_predicate_regexp_global_init(void);

/* Operation things */

typedef enum { OP_LT, OP_GT, OP_GE, OP_LE, OP_EQ, OP_NE } operation;
int strop(const char * const args);

/* Return length of operation in string. ie; "<=" (OP_LE) == 2 */
#define OP_LEN(op) ((op == OP_GT || op == OP_LT) ? 1 : 2)

#define OP_RUN(op, cmpval, retvar) \
    switch (op) { \
      case OP_LT: retvar = (cmpval < 0); break; \
      case OP_GT: retvar = (cmpval > 0); break; \
      case OP_GE: retvar = (cmpval >= 0); break; \
      case OP_LE: retvar = (cmpval <= 0); break; \
      case OP_EQ: retvar = (cmpval == 0); break; \
      case OP_NE: retvar = (cmpval != 0); break; \
    } 

typedef struct grok_predicate_regexp {
  pcre *re;
  char *pattern;
  int negative_match;
} grok_predicate_regexp_t;

typedef struct grok_predicate_numcompare {
  enum { DOUBLE, LONG } type;
  operation op;
  union {
    long lvalue;
    double dvalue;
  } u;
} grok_predicate_numcompare_t;

typedef struct grok_predicate_strcompare {
  operation op;
  char *value;
} grok_predicate_strcompare_t;

int grok_predicate_regexp(grok_t *grok, grok_capture_t *gct,
                          const char *subject, int start, int end);
int grok_predicate_numcompare(grok_t *grok, grok_capture_t *gct,
                              const char *subject, int start, int end);
int grok_predicate_strcompare(grok_t *grok, grok_capture_t *gct,
                              const char *subject, int start, int end);

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

  //fprintf(stderr, "Compiled regex for '%s': '%s'\n", gct->name, gprt->pattern);
  gct->predicate_func = grok_predicate_regexp;
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

int grok_predicate_numcompare_init(grok_t *grok, grok_capture_t *gct,
                                   const char *args) {
  grok_predicate_numcompare_t *gpnt;
  int pos;

  gpnt = calloc(1, sizeof(grok_predicate_numcompare_t));

  gpnt->op = strop(args);
  pos = OP_LEN(gpnt->op);

  /* Optimize and use long type if the number is not a float (no period) */
  if (strchr(args, '.') == NULL) {
    gpnt->type = LONG;
    gpnt->u.lvalue = strtol(args + pos, NULL, 0);
  } else {
    gpnt->type = DOUBLE;
    gpnt->u.dvalue = strtod(args + pos, NULL);
    //fprintf(stderr, "numcompare_init: %s %f\n", args + pos, gpnt->u.dvalue);
  }

  gct->predicate_func = grok_predicate_numcompare;
  gct->extra = gpnt;
}

int grok_predicate_numcompare(grok_t *grok, grok_capture_t *gct,
                              const char *subject, int start, int end) {
  grok_predicate_numcompare_t *gpnt = (grok_predicate_numcompare_t *)gct->extra;
  int ret;

  //fprintf(stderr, "numcompare: %*.s\n", (end - start), subject + start);

  if (gpnt->type == DOUBLE) {
    double a = strtod(subject + start, NULL);
    double b = gpnt->u.dvalue;
    OP_RUN(gpnt->op, a - b, ret);
  } else {
    long a = strtol(subject + start, NULL, 0);
    long b = gpnt->u.lvalue;
    //printf("%ld vs %ld\n", a, b);
    OP_RUN(gpnt->op, a - b, ret);
  }

  /* grok predicates should return 0 for success and comparisons return 1 for
   * success */
  return ret == 0;
}

int grok_predicate_strcompare_init(grok_t *grok, grok_capture_t *gct,
                                   const char *args) {
  grok_predicate_strcompare_t *gpst;
  int pos;

  /* XXX: ALLOC */
  gpst = calloc(1, sizeof(grok_predicate_strcompare_t));

  gpst->op = strop(args);
  pos = OP_LEN(gpst->op);

  /* XXX: ALLOC */
  gpst->value = strdup(args);

  gct->predicate_func = grok_predicate_strcompare;
  gct->extra = gpst;
}

int grok_predicate_strcompare(grok_t *grok, grok_capture_t *gct,
                              const char *subject, int start, int end) {
  grok_predicate_strcompare_t *gpst = (grok_predicate_strcompare_t *)gct->extra;
  int ret;

  OP_RUN(gpst->op,
         strncmp(subject + start, gpst->value, (end - start)),
         ret);

  /* grok predicates should return 0 for success and comparisons return 1 for
   * success */
  return ret == 0;
}

int strop(const char * const args) {
  switch (args[0]) {
    case '<':
      if (args[1] == '=') return OP_LE;
      else return OP_LT;
      break;
    case '>':
      if (args[1] == '=') return OP_GE;
      else return OP_GT;
      break;
    case '=':
      if (args[1] == '=') return OP_EQ;
      else {
        fprintf(stderr, "Invalid predicate: '%s'\n", args);
        return -1;
      }
      break;
    case '!':
      if (args[1] == '=') return OP_NE;
      else {
        fprintf(stderr, "Invalid predicate: '%s'\n", args);
        return -1;
      }
      break;
    default:
      fprintf(stderr, "Invalid predicate: '%s'\n", args);
  }

  return -1;
}
