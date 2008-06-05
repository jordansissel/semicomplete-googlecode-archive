
#include <search.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


typedef struct foo {
  const char *name;
  int value;
} foo_t;

int cmp(const void *a, const void *b) {
  foo_t *fa, *fb;

  fa = (foo_t*)a;
  fb = (foo_t*)b;
  return strcmp(fa->name, fb->name);
}

void w(const void *node, const VISIT which, const int depth) {
  foo_t *f;
  f = (foo_t *)node;
  printf("%s: %d\n", f->name, f->value);
}

int main() {
  int i, *ptr;
  void *root = NULL;
  const void *ret;
  foo_t *val;

  val = calloc(1, sizeof(foo_t));
  val->name = strdup("one");
  val->value = 1;
  printf("name: %s\n", val->name);
  ret = tsearch(val, &root, cmp);
  printf("retname: %s\n", (*(foo_t **)ret)->name);

  twalk(root, w);
  return 0;

}

  
