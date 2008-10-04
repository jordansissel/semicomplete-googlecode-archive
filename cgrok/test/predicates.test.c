#include "grok.h"

#define INIT \
    grok_t grok; \
    grok_init(&grok);

#define ASSERT_MATCHFAIL(str) CU_ASSERT(grok_exec(&grok, str, NULL) < 0)
#define ASSERT_MATCHOK(str) CU_ASSERT(grok_exec(&grok, str, NULL) >= 0)


void test_grok_with_predicate_compile_succeeds(void) {
  INIT;
  grok_patterns_import_from_string(&grok, "WORD \\b\\w+\\b");

  CU_ASSERT(grok_compile(&grok, "%{WORD=~/test/}") == 0);
  CU_ASSERT(grok_compile(&grok, "%{WORD>30}") == 0);

  grok_free(&grok);
}

void test_grok_with_numcompare_gt_normal(void) {
  INIT;
  grok_patterns_import_from_file(&grok, "../pcregrok_patterns");

  grok_compile(&grok, "^%{NUMBER>10}$");

  ASSERT_MATCHFAIL("0");
  ASSERT_MATCHFAIL("1");
  ASSERT_MATCHFAIL("9");
  ASSERT_MATCHFAIL("10");
  ASSERT_MATCHFAIL("5.5");
  ASSERT_MATCHFAIL("0.2");
  ASSERT_MATCHFAIL("9.95");

  ASSERT_MATCHOK("10.1")
  ASSERT_MATCHOK("10.2")
  ASSERT_MATCHOK("11.2")
  ASSERT_MATCHOK("4425.334")
  ASSERT_MATCHOK("11");
  ASSERT_MATCHOK("15");
}

void test_grok_with_numcompare_gt_hex(void) {
  INIT;
  grok_patterns_import_from_file(&grok, "../pcregrok_patterns");

  grok_compile(&grok, "%{BASE16FLOAT>0x000A}");

  ASSERT_MATCHFAIL("0");
  ASSERT_MATCHFAIL("1");
  ASSERT_MATCHFAIL("9");
  ASSERT_MATCHFAIL("10");
  ASSERT_MATCHFAIL("0x05");
  ASSERT_MATCHFAIL("0x0A");

  ASSERT_MATCHOK("11");
  ASSERT_MATCHOK("15");
  ASSERT_MATCHOK("0x0B");
  ASSERT_MATCHOK("0xB");
  ASSERT_MATCHOK("0xFF");
}
