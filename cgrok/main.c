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

  if (argc != 2) {
    printf("Usage: %s <regex>\n", argv[0]);
    return 1;
  }

  grok_compile(&grok, argv[1]);
 
  while (fgets(buf, BUFSIZE, stdin)) {
    int ret;
    ret = grok_exec(&grok, buf, &gm);

    if (ret >= 0) {
      char *name;
      const char *data;
      int namelen, datalen;
      void *handle;
      handle = grok_match_walk_init(&gm);
      while (grok_match_walk_next(&gm, handle, &name, &namelen, &data, &datalen) == 0) {
        printf("%.*s => %.*s\n", namelen, name, datalen, data);
        free(name);
      };
      grok_match_walk_end(&gm, handle);
    }
  }

  grok_free(&grok);
  return 0;
}
