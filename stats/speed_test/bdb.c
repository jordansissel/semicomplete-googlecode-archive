#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <db.h>

static int ITERSIZE;

int main(int argc, char **argv) {
  DB *dbp;
  int ret;
  int pagesize = 4096;
  int cachesize = 1<<20;

  if (argc < 2) {
    printf("Usage: prog <numinserts> [pagesize] [cachesize]\n");
    exit(1);
  }

  if (argc >= 3)
    pagesize = atoi(argv[2]);
  if (argc >= 4)
    cachesize = atoi(argv[3]);

  ITERSIZE = atoi(argv[1]);

  ret = db_create(&dbp, NULL, 0);
  if (ret != 0) {
    fprintf(stderr, "db_create failed\n");
    exit(1);
  }

  dbp->set_pagesize(dbp, pagesize);
  dbp->set_cachesize(dbp, 0, cachesize, 0);
  dbp->open(dbp, NULL, NULL, NULL, DB_BTREE, DB_CREATE, 0);

  DBT key, value;
  char *key_str;

  key_str = malloc(20);
  if (key_str == NULL) {
    perror("malloc");
    exit(1);
  }

  int i;
  memset(&key, 0, sizeof(DBT));
  memset(&value, 0, sizeof(DBT));
  memset(key_str, 0, 20);
  for (i = 0; i < ITERSIZE; i++) {
    snprintf(key_str, 20, "%d", i);
    key.data = key_str;
    key.size = strlen(key_str);
    value.data = "foo";
    value.size = strlen("foo");
    dbp->put(dbp, NULL, &key, &value, 0);
  }

  dbp->close(dbp, 0);
  exit(0);

  DBC *cursorp;

  dbp->cursor(dbp, NULL, &cursorp, 0);
  int count = 0;
  while ((ret = cursorp->c_get(cursorp, &key, &value, DB_NEXT)) == 0) {
    if (!strcmp(key.data, "foo")) {
      printf("Unexpected result: '%s' / wanted 'foo'\n", key.data);
    }
    count++;
  }
  if (count != ITERSIZE)
    printf("count != itersize: %d vs %d\n", count, ITERSIZE);

  cursorp->c_close(cursorp);

  dbp->close(dbp, 0);
}
