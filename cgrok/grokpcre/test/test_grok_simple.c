#include "../grok.h"

void test_grok_pcre_compile_succeeds(void) {
  grok_t grok;
  grok_init(&grok);

  CU_ASSERT(grok_compile(&grok, "\\w+") == 0);
  CU_ASSERT(grok_compile(&grok, "[") != 0);
}

void test_grok_pcre_match(void) {
  grok_t grok;
  grok_init(&grok);

  grok_compile(&grok, "[a-z]+");

  CU_ASSERT(grok_exec(&grok, "foo", NULL) >= 0);
  CU_ASSERT(grok_exec(&grok, "  one two  ", NULL) >= 0);
  CU_ASSERT(grok_exec(&grok, "...", NULL) < 0);
  CU_ASSERT(grok_exec(&grok, "1234", NULL) < 0);

  grok_free(&grok);
}
