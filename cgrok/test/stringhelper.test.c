#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "stringhelper.h"

void test_unmoving_insertion(void) {
  char *source = calloc(1, 1024);
  char *dest = calloc(1, 1024);
  int slen = -1, dlen = -1, dalloc = 1024;

  sprintf(source, "world");
  sprintf(dest, "hello there");

  substr_replace(&dest, &dlen, &dalloc, 6, strlen(dest), source, slen);
  CU_ASSERT(!strcmp(dest, "hello world"));
  CU_ASSERT(dalloc == 1024);
  CU_ASSERT(dlen == 11);

  free(source);
  free(dest);
}

void test_alloc_and_insert_from_null_dest(void) {
  char *source = "hello world";
  char *dest = NULL;
  int dlen = 0, dalloc = 0;

  substr_replace(&dest, &dlen, &dalloc, 0, 0, source, -1);

  CU_ASSERT(!strcmp(dest, source));
  CU_ASSERT(dlen == strlen(source));
  CU_ASSERT(dalloc > dlen);

  free(dest);
}

void test_remove(void) {
  char *source = strdup("hello world");
  int len = strlen(source);
  int size = len + 1;

  //printf("\n--\n%s\n", source);
  substr_replace(&source, &len, &size, 5, len, "", 0);
  //printf("\n--\n%s\n", source);
  CU_ASSERT(!strcmp(source, "hello"));
}

void test_lenchange(void) {
  char *source = strdup("hello world test");
  int len = strlen(source);
  int size = len + 1;

  CU_ASSERT(len == 16);
  substr_replace(&source, &len, &size, 5, len, "", 0);
  CU_ASSERT(len == 5);
}
