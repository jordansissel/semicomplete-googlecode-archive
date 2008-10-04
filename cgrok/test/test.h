#define ASSERT_MATCHFAIL(str) CU_ASSERT(grok_exec(&grok, str, NULL) < 0)
#define ASSERT_MATCHOK(str) CU_ASSERT(grok_exec(&grok, str, NULL) >= 0)
#define ASSERT_COMPILEOK(str) CU_ASSERT(grok_compile(&grok, str) == 0) 
#define ASSERT_COMPILEFAIL(str) CU_ASSERT(grok_compile(&grok, str) != 0) 

/* Helpers */

#define INIT \
  grok_t grok; \
  grok_init(&grok);

#define CLEANUP \
  grok_free(&grok);

#define IMPORT_PATTERNS_FILE grok_patterns_import_from_file(&grok, "../pcregrok_patterns")
