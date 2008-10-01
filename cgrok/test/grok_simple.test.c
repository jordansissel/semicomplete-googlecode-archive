#include "grok.h"

void test_grok_pcre_compile_succeeds(void) {
  grok_t grok;
  grok_init(&grok);

  CU_ASSERT(grok_compile(&grok, "\\w+") == 0);
  CU_ASSERT(grok_compile(&grok, "[") != 0);

  grok_free(&grok);
}

void test_grok_pcre_match(void) {
  grok_t grok;
  grok_init(&grok);

  CU_ASSERT(grok_compile(&grok, "[a-z]+") == 0);

  CU_ASSERT(grok_exec(&grok, "foo", NULL) >= 0);
  CU_ASSERT(grok_exec(&grok, "  one two  ", NULL) >= 0);
  CU_ASSERT(grok_exec(&grok, "...", NULL) < 0);
  CU_ASSERT(grok_exec(&grok, "1234", NULL) < 0);

  grok_free(&grok);
}

void test_grok_match_with_patterns(void) {
  grok_t grok;
  grok_init(&grok);

  grok_patterns_import_from_string(&grok, "WORD \\b\\w+\\b");

  CU_ASSERT(grok_compile(&grok, "%{WORD}") == 0);

  CU_ASSERT(grok_exec(&grok, "testing", NULL) >= 0);
  CU_ASSERT(grok_exec(&grok, "  one two  ", NULL) >= 0);
  CU_ASSERT(grok_exec(&grok, "---", NULL) < 0);
  CU_ASSERT(grok_exec(&grok, "-.", NULL) < 0);

  grok_free(&grok);
}

void test_grok_match_substr(void) {
  grok_t grok;
  grok_match_t gm;
  grok_init(&grok);
  
  CU_ASSERT(grok_compile(&grok, "\\w+ world") == 0);

  CU_ASSERT(grok_exec(&grok, "something hello world", &gm) >= 0);
  CU_ASSERT(grok.pcre_capture_vector[0] == 10); // start of match
  CU_ASSERT(grok.pcre_capture_vector[1] == 21); // end of match

  // XXX: make function:
  // int grok_match_string(grok_t, grok_match_t, char **matchstr, int *matchlen)
  // verify the matched string is 'hello world'
  CU_ASSERT(!strncmp("hello world",
                     gm.subject + grok.pcre_capture_vector[0],
                     grok.pcre_capture_vector[1] - grok.pcre_capture_vector[0]));
  grok_free(&grok);
}
