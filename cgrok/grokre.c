#include <pcre.h>
#include <assert.h>
#include <string.h> 
#include <stdlib.h>
#include <stdio.h>
#include <search.h>
#include <db.h>

#include "grok.h"
#include "predicates.h"
#include "stringhelper.h"

/* global, static variables */

#define CAPTURE_ID_LEN 4
#define CAPTURE_FORMAT "%04x"

/* internal functions */
static char *grok_pattern_expand(grok_t *grok);
static void grok_study_capture_map(grok_t *grok);

static void grok_capture_add_predicate(grok_t *grok, int capture_id,
                                       const char *predicate);

//static grok_capture_t* grok_match_get_named_capture(const grok_match_t *gm, 
                                                    //const char *name);

void grok_free(grok_t *grok) {
  if (grok->re != NULL)
    pcre_free(grok->re);

  if (grok->full_pattern != NULL)
    free(grok->full_pattern);

  if (grok->pcre_capture_vector != NULL)
    free(grok->pcre_capture_vector);

  if (grok->captures_by_id != NULL)
    free(grok->captures_by_id);
  if (grok->captures_by_name != NULL)
    free(grok->captures_by_name);

}

int grok_compile(grok_t *grok, const char *pattern) {
  grok_log(grok, LOG_COMPILE, "Compiling '%s'", pattern);
  grok->pattern = pattern;
  grok->full_pattern = grok_pattern_expand(grok);

  grok->re = pcre_compile(grok->full_pattern, 0, 
                          &grok->pcre_errptr, &grok->pcre_erroffset,
                          NULL);

  if (grok->re == NULL) {
    fprintf(stderr, "Regex error: %s\n", grok->pcre_errptr);
    return 1;
  }

  pcre_fullinfo(grok->re, NULL, PCRE_INFO_CAPTURECOUNT, &grok->pcre_num_captures);
  grok->pcre_num_captures++; /* include the 0th group */
  grok->pcre_capture_vector = calloc(3 * grok->pcre_num_captures, sizeof(int));

  /* Walk grok->captures_by_id.
   * For each, ask grok->re what stringnum it is */
  grok_study_capture_map(grok);

  return 0;
}

int grok_exec(grok_t *grok, const char *text, grok_match_t *gm) {
  int ret;
  pcre_extra pce;
  pce.flags = PCRE_EXTRA_CALLOUT_DATA;
  pce.callout_data = grok;

  ret = pcre_exec(grok->re, &pce, text, strlen(text), 0, 0,
                  grok->pcre_capture_vector, grok->pcre_num_captures * 3);
  if (ret < 0) {
    switch (ret) {
      case PCRE_ERROR_NOMATCH:
        break;
      case PCRE_ERROR_NULL:
        fprintf(stderr, "Null error, one of the arguments was null?\n");
        break;
      case PCRE_ERROR_BADOPTION:
        fprintf(stderr, "badoption\n");
        break;
      case PCRE_ERROR_BADMAGIC:
        fprintf(stderr, "badmagic\n");
        break;
    }
    return ret;
  }

  if (gm != NULL) {
    gm->grok = grok;
    gm->subject = text;
  }

  return ret;
}

/* XXX: This method is pretty long; split it up? */
char *grok_pattern_expand(grok_t *grok) {
  int capture_id = 0;
  int offset = 0;
  int *capture_vector = NULL;

  int full_len = -1;
  int full_size = -1;
  char *full_pattern = NULL;
  char capture_id_str[CAPTURE_ID_LEN + 1];

  const char *patname = NULL;
  const char *longname = NULL;

  grok_log(grok, LOG_REGEXPAND, "Expanding pattern '%s'", grok->pattern);

  capture_vector = calloc(3 * g_pattern_num_captures, sizeof(int));
  full_len = strlen(grok->pattern);
  full_size = full_len + 1;
  full_pattern = calloc(1, full_size);
  strncpy(full_pattern, grok->pattern, full_len);

  while (pcre_exec(g_pattern_re, NULL, full_pattern, full_len, offset, 
                   0, capture_vector, g_pattern_num_captures * 3) >= 0) {
    int start, end, matchlen;
    const char *pattern_regex;
    int patname_len;

    start = capture_vector[0];
    end = capture_vector[1];
    matchlen = end - start;

    pcre_get_substring(full_pattern, capture_vector, g_pattern_num_captures,
                       g_cap_pattern, &patname);
    patname_len = capture_vector[g_cap_pattern * 2 + 1] \
                  - capture_vector[g_cap_pattern * 2];

    pattern_regex = grok_pattern_find(grok, patname, patname_len);
    if (pattern_regex == NULL) {
      offset = end;
    } else {
      int regexp_len = strlen(pattern_regex);
      int has_predicate = (capture_vector[g_cap_predicate * 2] >= 0);
      grok_capture gct;
      grok_capture_init(grok, &gct);

      pcre_get_substring(full_pattern, capture_vector, g_pattern_num_captures,
                         g_cap_name, &longname);

      snprintf(capture_id_str, CAPTURE_ID_LEN + 1, CAPTURE_FORMAT, capture_id);

      /* Add this capture to the list of captures */
      gct.id = capture_id;
      gct.name = (char *)longname; /* XXX: CONST PROBLEM */
      grok_capture_add(grok, &gct);
      pcre_free_substring(longname);

      /* Invariant, full_pattern actual len must always be full_len */
      assert(strlen(full_pattern) == full_len);

      /* if a predicate was given, add (?C1) to callout when the match is made,
       * so we can test it further */
      if (has_predicate) {
        const char *predicate = NULL;
        grok_log(grok, LOG_REGEXPAND, "Predicate found in '%.*s'",
                 matchlen, full_pattern + start);
        pcre_get_substring(full_pattern, capture_vector, g_pattern_num_captures,
                           g_cap_predicate, &predicate);
        grok_capture_add_predicate(grok, capture_id, predicate);
        substr_replace(&full_pattern, &full_len, &full_size,
                       end, -1, "(?C1)", 5);
        pcre_free_substring(predicate);
      }

      /* Replace %{FOO} with (?<>). '5' is strlen("(?<>)") */
      substr_replace(&full_pattern, &full_len, &full_size,
                     start, end, "(?<>)", 5);

      /* Insert the capture id into (?<FOO>) */
      substr_replace(&full_pattern, &full_len, &full_size,
                     start + 3, -1,
                     capture_id_str, CAPTURE_ID_LEN);

      /* Insert the pattern into (?<FOO>pattern) */
      /* 3 = '(?<', 4 = strlen(capture_id_str), 1 = ")" */
      substr_replace(&full_pattern, &full_len, &full_size, 
                     start + 3 + CAPTURE_ID_LEN + 1, -1, 
                     pattern_regex, regexp_len);


      /* Invariant, full_pattern actual len must always be full_len */
      assert(strlen(full_pattern) == full_len);
      
      /* Move offset to the start of the regexp pattern we just injected.
       * This is so when we iterate again, we can process this new pattern
       * to see if the regexp included itself any %{FOO} things */
      offset = start;
      capture_id++;
    }

    if (patname != NULL) {
      pcre_free_substring(patname);
      patname = NULL;
    }
  }
  //fprintf(stderr, "Expanded: %s\n", full_pattern);
  free(capture_vector);

  return full_pattern;
}

static void grok_capture_add_predicate(grok_t *grok, int capture_id,
                                       const char *predicate) {
  grok_capture gct;
  grok_capture_init(grok, &gct);

  grok_log(grok, LOG_PREDICATE, "Adding predicate '%s' to capture %d",
           predicate, capture_id);

  grok_capture_get_by_id(grok, capture_id, &gct);

  /* Compile the predicate into something useful */
  /* So far, predicates are just an operation and an argument */
  /* predicate_func(capture_str, args) ??? */

  /* skip leading whitespace */
  predicate += strspn(predicate, " \t");

  if (!strncmp(predicate, "=~", 2) || !strncmp(predicate, "!~", 2)) {
    grok_predicate_regexp_init(grok, &gct, predicate);
  } else if (strchr("!<>=", predicate[0]) != NULL) {
    grok_predicate_numcompare_init(grok, &gct, predicate);
  } else if ((predicate[0] == '$') 
             && (strchr("!<>=", predicate[1]) != NULL)) {
    grok_predicate_strcompare_init(grok, &gct, predicate);
  } else {
    fprintf(stderr, "unknown pred: %s\n", predicate);
  }
}

/*
* static grok_capture_t* grok_match_get_named_capture(const grok_match_t *gm, 
                                                    * const char *name) {
  * grok_capture_t key;
  * void *result = NULL;
* 
  * key.name = name;
  * result = tfind(&key, &(gm->grok->captures_by_name), grok_capture_cmp_name);
  * if (result == NULL)
    * return NULL;
* 
  * return *(grok_capture_t **)result;
* }
*/

#if 0
int grok_match_get_named_substring(const grok_match_t *gm, const char *name,
                                   const char **substr, int *len) {
  grok_capture gct;
  int start, end;

  grok_log(gm->grok, LOG_EXEC, "Fetching named capture: %s", name);
  //grok_match_get_named_capture(gm, name, &gct);
  if (gct == NULL) {
    grok_log(gm->grok, LOG_EXEC, "Named capture '%s' not found", name);
    *substr = NULL;
    *len = 0;
    return -1;
  }

  start = (gm->grok->pcre_capture_vector[gct->pcre_capture_number * 2]);
  end = (gm->grok->pcre_capture_vector[gct->pcre_capture_number * 2 + 1]);
  grok_log(gm->grok, LOG_EXEC, "Capture '%s' is %d -> %d of '%s'",
           name, start, end, gm->subject);
  *substr = gm->subject + start;
  *len = (end - start);

  return 0;
}
#endif

static void grok_study_capture_map(grok_t *grok) {
  char *nametable;
  grok_capture gct;
  int nametable_size;
  int nametable_entrysize;
  int i = 0;
  int offset = 0;
  int stringnum;
  int capture_id;

  grok_capture_init(grok, &gct);

  pcre_fullinfo(grok->re, NULL, PCRE_INFO_NAMECOUNT, &nametable_size);
  pcre_fullinfo(grok->re, NULL, PCRE_INFO_NAMEENTRYSIZE, &nametable_entrysize);
  pcre_fullinfo(grok->re, NULL, PCRE_INFO_NAMETABLE, &nametable);

  for (i = 0; i < nametable_size; i++) {
    offset = i * nametable_entrysize;
    stringnum = (nametable[offset] << 8) + nametable[offset + 1];
    sscanf(nametable + offset + 2, CAPTURE_FORMAT, &capture_id);
    grok_capture_get_by_id(grok, capture_id, &gct);
    gct.pcre_capture_number = stringnum;
    /* update the database with the new data */
    grok_capture_add(grok, &gct);
  }
}
