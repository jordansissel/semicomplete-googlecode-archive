#include <stdio.h>
#include <string.h>
#include "grok.h"

#include <sys/select.h>

#ifndef LOGMASK
#define LOGMASK LOG_ALL
#endif

void print_all(grok_match_t *gm) {
  char *name;
  const char *data;
  int namelen, datalen;
  void *handle;
  handle = grok_match_walk_init(gm);
  while (grok_match_walk_next(gm, handle, &name, &namelen, &data, &datalen) == 0) {
    printf("%.*s => %.*s\n", namelen, name, datalen, data);
    free(name);
  };
  grok_match_walk_end(gm, handle);
}

void print_name(grok_match_t *gm, char *name) {
  const char *str = NULL;
  int len = 0;
  int ret;
  ret = grok_match_get_named_substring(gm, name, &str, &len);
  if (ret == 0) {
    printf("%s = %.*s\n", name, len, str);
  }
}

int main(int argc, char **argv) {
  grok_t grok;
  grok_match_t gm;
  #define BUFSIZE 4096
  char buf[BUFSIZE];
  grok_init(&grok);
  grok.logmask = LOG_ALL;

  grok_patterns_import_from_file(&grok, "./pcregrok_patterns");

  if (argc < 2 || argc > 3) {
    printf("Usage: %s <regex> [name]\n", argv[0]);
    return 1;
  }

  grok_compile(&grok, argv[1]);
 
  while (fgets(buf, BUFSIZE, stdin)) {
    int ret;
    buf[strlen(buf) - 1] = '\0'; /* kill newline */
    ret = grok_exec(&grok, buf, &gm);

    if (ret >= 0) {
      if (argc == 3) {
        print_name(&gm, argv[2]);
      } else {
        print_all(&gm);
      }
    }
  }

  grok_free(&grok);
  return 0;
}
