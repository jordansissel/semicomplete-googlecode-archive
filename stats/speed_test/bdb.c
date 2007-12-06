#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <db.h>

int main() {
  DB *dbp;
  int ret;

  ret = db_create(&dbp, NULL, 0);
  if (ret != 0) {
    fprintf(stderr, "db_create failed\n");
    exit(1);
  }

  dbp->open(dbp, NULL, "/tmp/c.db", NULL, DB_BTREE, DB_CREATE, 0);


  dbp->close(dbp, 0);
}
