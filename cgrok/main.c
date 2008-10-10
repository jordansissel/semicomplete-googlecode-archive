#include <stdio.h>
#include <string.h>
#include "grok.h"

#include <sys/select.h>

#ifndef LOGMASK
#define LOGMASK LOG_ALL
#endif

int main(int argc, char **argv) {
  grok_t grok;
  grok_match_t gm;
  #define BUFSIZE 4096
  char buf[BUFSIZE];
  grok_init(&grok);
  grok.logmask = LOGMASK;

  grok_patterns_import_from_file(&grok, "./pcregrok_patterns");

  if (argc != 3) {
    printf("Usage: %s <regex> <name>\n", argv[0]);
    return 1;
  }

  grok_compile(&grok, argv[1]);
 
  while (fgets(buf, BUFSIZE, stdin)) {
    int ret;
    ret = grok_exec(&grok, buf, &gm);

    if (ret >= 0) {
      const char *data;
      int len;
      grok_match_get_named_substring(&gm, argv[2], &data, &len);
      printf("%.*s\n", len, data);
    }
  }

  grok_free(&grok);
  return 0;
}
