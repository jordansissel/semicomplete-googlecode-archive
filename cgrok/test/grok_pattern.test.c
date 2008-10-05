#include "grok.h"
#include "test.h"

void test_grok_pattern_add_and_find_work(void) {
  INIT;
  const char *regexp;

  grok_pattern_add(&grok, "WORD", 5, "\\w+", 4);
  grok_pattern_add(&grok, "TEST", 5, "TEST", 5);

  regexp = grok_pattern_find(&grok, "WORD", 5);
  CU_ASSERT(!strcmp(regexp, "\\w+"));

  regexp = grok_pattern_find(&grok, "TEST", 5);
  CU_ASSERT(!strcmp(regexp, "TEST"));
}

void test_pattern_parse(void) {
  const char *name, *regexp;
  size_t name_len, regexp_len;

  _pattern_parse_string("WORD \\w+", &name, &name_len, &regexp, &regexp_len);
  CU_ASSERT(!strncmp(name, "WORD", name_len));
  CU_ASSERT(!strncmp(regexp, "\\w+", regexp_len));

  _pattern_parse_string("   NUM    numtest", &name, &name_len, &regexp, &regexp_len);
  CU_ASSERT(!strncmp(name, "NUM", name_len));
  CU_ASSERT(!strncmp(regexp, "numtest", regexp_len));
  //printf("\n-\n'%.*s'\n", regexp_len, regexp);
  //printf("\n-\n'%.*s'\n", name_len, name);

  _pattern_parse_string(" 	 NUM 		 numtest", &name, &name_len, &regexp, &regexp_len);
  CU_ASSERT(!strncmp(name, "NUM", name_len));
  CU_ASSERT(!strncmp(regexp, "numtest", regexp_len));
}

void test_pattern_import_from_string(void) {
  INIT;
  const char *buf = "WORD \\w+\n"
     "TEST test\n"
     "# This is a comment\n"
     "FOO bar\n";

  buf = strdup(buf);
  grok_patterns_import_from_string(&grok, buf);

  //CU_ASSERT(!strcmp(grok_pattern_find(&grok, "WORD", 5), "\\w+"));
  //CU_ASSERT(!strcmp(grok_pattern_find(&grok, "TEST", 5), "test"));
  //CU_ASSERT(!strcmp(grok_pattern_find(&grok, "FOO", 4), "bar"));

  

  
}
