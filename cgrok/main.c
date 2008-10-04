#include <stdio.h>
#include <string.h>
#include "grok.h"

int main(int argc, char **argv) {
  grok_t grok;
  grok_match_t gm;
  char buf[1024];
  grok_init(&grok);
  grok.logmask = LOG_ALL;

  grok_patterns_import_from_file(&grok, "./pcregrok_patterns");

  if (argc != 2) {
    printf("Usage: %s <regex>\n", argv[0]);
    return 1;
  }

  printf("Compile: %d\n", grok_compile(&grok, argv[1]));

 
  while (fgets(buf, 1024, stdin)) {
    int start, end, ret;
    ret = grok_exec(&grok, buf, &gm);
    start = grok.pcre_capture_vector[0];
    end = grok.pcre_capture_vector[1];
    printf("Match: %d\n", ret);
    if (ret >= 0)
      printf("Substr: '%.*s'\n", end - start, buf + start);
  }

  return 0;
}
