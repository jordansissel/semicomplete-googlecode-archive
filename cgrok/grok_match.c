#include "grok.h"

int grok_match_get_named_capture(const grok_match_t *gm,
                                 const char *name, grok_capture *gct) {
  grok_capture_init(gm->grok, gct);
  return grok_capture_get_by_name(gm->grok, name, gct);
}

int grok_match_get_named_substring(const grok_match_t *gm, const char *name,
                                   const char **substr, int *len) {
  int start, end;
  grok_capture gct;

  grok_log(gm->grok, LOG_EXEC, "Fetching named capture: %s", name);
  if (grok_match_get_named_capture(gm, name, &gct) != 0) {
    grok_log(gm->grok, LOG_EXEC, "Named capture '%s' not found", name);
    *substr = NULL;
    *len = 0;
    return -1;
  }

  start = (gm->grok->pcre_capture_vector[gct.pcre_capture_number * 2]);
  end = (gm->grok->pcre_capture_vector[gct.pcre_capture_number * 2 + 1]);
  grok_log(gm->grok, LOG_EXEC, "Capture '%s' is %d -> %d of string '%s'",
           name, start, end, gm->subject);
  *substr = gm->subject + start;
  *len = (end - start);

  grok_capture_free(&gct);
  return 0;
}
