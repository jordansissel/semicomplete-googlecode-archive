#include <stdio.h>
#include <string.h>
#include "grok.h"

#include <sys/select.h>


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

  grok_compile(&grok, argv[1]);
 
  while (fgets(buf, 4095, stdin)) {
    //int start, end, ret;
    int ret;
    buf[4096] = '\0';
    //ret = grok_exec(&grok, buf, &gm);
    ret = grok_exec(&grok, buf, NULL);
    //start = grok.pcre_capture_vector[0];
    //end = grok.pcre_capture_vector[1];
    if (ret >= 0) {
      printf("%s", buf);
    }
  }

  grok_free(&grok);
  return 0;
}
