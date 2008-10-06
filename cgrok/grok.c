#include "grok.h"
#include "grok_capture.h"

/* These guys are initialized once ever. */
int g_grok_global_initialized = 0;
pcre *g_pattern_re = NULL;
int g_pattern_num_captures = 0;
int g_cap_name = 0;
int g_cap_pattern = 0;
int g_cap_subname = 0;
int g_cap_predicate = 0;

/* pattern to match %{FOO:BAR} */
/* or %{FOO<=3} */
#define PATTERN_REGEX "%{" \
                        "(?<name>" \
                          "(?<pattern>[A-z0-9._-]+)" \
                          "(?::(?<subname>[A-z0-9._-]+))?" \
                        ")" \
                        "(?<predicate>\\s*(?:[$]?=[<>=~]|![=~]|[$]?[<>])\\s*[^}]+)?" \
                      "}"
#define CAPTURE_ID_LEN 4
#define CAPTURE_FORMAT "%04x"

static int grok_pcre_callout(pcre_callout_block *pcb);

void grok_init(grok_t *grok) {
  pcre_callout = grok_pcre_callout;

  grok->re = NULL;
  grok->full_pattern = NULL;
  grok->pcre_capture_vector = NULL;
  grok->pcre_num_captures = 0;
  grok->max_capture_num = 0;
  grok->pcre_errptr = NULL;
  grok->pcre_erroffset = 0;
  grok->logmask = 0;

#ifndef GROK_TEST_NO_PATTERNS
  db_create(&grok->patterns, NULL, 0);
  grok->patterns->open(grok->patterns, NULL, NULL, "patterns",
                       DB_BTREE, DB_CREATE, 0);
#endif /* GROK_TEST_NO_PATTERNS */

#ifndef GROK_TEST_NO_CAPTURE
  db_create(&grok->captures_by_id, NULL, 0);
  db_create(&grok->captures_by_name, NULL, 0);
  db_create(&grok->captures_by_capture_number, NULL, 0);

  grok->captures_by_id->open(grok->captures_by_id, NULL, NULL,
                             "captures_by_id", DB_BTREE, DB_CREATE, 0);
  grok->captures_by_name->open(grok->captures_by_name, NULL, NULL,
                               "captures_by_name", DB_BTREE, DB_CREATE, 0);
  grok->captures_by_capture_number->open(grok->captures_by_capture_number,
                                         NULL, NULL, "captures_by_capture_number",
                                         DB_BTREE, DB_CREATE, 0);

  grok->captures_by_id->associate(grok->captures_by_id, NULL, 
                                  grok->captures_by_name,
                                  _db_captures_by_name_key, 0);
  grok->captures_by_id->associate(grok->captures_by_id, NULL,
                                  grok->captures_by_capture_number,
                                  _db_captures_by_capture_number, 0);
#endif /* GROK_TEST_NO_CAPTURE */

  if (g_grok_global_initialized == 0) {
    /* do first initalization */
    g_grok_global_initialized = 1;
    g_pattern_re = pcre_compile(PATTERN_REGEX, 0,
                                &grok->pcre_errptr,
                                &grok->pcre_erroffset,
                                NULL);
    if (g_pattern_re == NULL) {
      fprintf(stderr, "Internal compiler error: %s\n", grok->pcre_errptr);
      fprintf(stderr, "Regexp: %s\n", PATTERN_REGEX);
      fprintf(stderr, "Position: %d\n", grok->pcre_erroffset);
    }

    pcre_fullinfo(g_pattern_re, NULL, PCRE_INFO_CAPTURECOUNT,
                  &g_pattern_num_captures);
    g_pattern_num_captures++; /* include the 0th group */
    g_cap_name = pcre_get_stringnumber(g_pattern_re, "name");
    g_cap_pattern = pcre_get_stringnumber(g_pattern_re, "pattern");
    g_cap_subname = pcre_get_stringnumber(g_pattern_re, "subname");
    g_cap_predicate = pcre_get_stringnumber(g_pattern_re, "predicate");
  }
}

static int grok_pcre_callout(pcre_callout_block *pcb) {
  //grok_t *grok = pcb->callout_data;
  //grok_capture *gct;
  //void *result;

  //printf("callout: %d\n", pcb->capture_last);

  //key.pcre_capture_number = pcb->capture_last;
  //result = tfind(&key, &(grok->captures_by_capture_number),
                 //grok_capture_cmp_capture_number);
  //assert(result != NULL);
  //gct = *(grok_capture_t **)result;
//
  //if (gct->predicate_func != NULL) {
    //int start, end;
    //start = pcb->offset_vector[ pcb->capture_last * 2 ];
    //end = pcb->offset_vector[ pcb->capture_last * 2 + 1];
    //return gct->predicate_func(grok, gct, pcb->subject, start, end);
  //}
//
  return 0;
}
