
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../stringhelper.h"

void test_substr_replace(void) {
  char *source = calloc(1, 1024);
  char *dest = calloc(1, 1024);
  int slen = -1, dlen = -1, salloc = 1024;

  sprintf(source, "world");
  sprintf(dest, "hello there");

  substr_replace(&dest, &slen, &salloc, 6, strlen(dest), source, dlen);
  printf("%s\n", dest);

  free(source);
  free(dest);
}

