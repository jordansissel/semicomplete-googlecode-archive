#include "grok.h"

void test_grok_with_predicate_compile_succeeds(void) {
  grok_t grok;
  grok_init(&grok);

  grok_patterns_import_from_string(&grok, "WORD \\b\\w+\\b");

  CU_ASSERT(grok_compile(&grok, "%{WORD=~/test/}") == 0);
  CU_ASSERT(grok_compile(&grok, "%{WORD>30}") != 0);

  grok_free(&grok);
}
