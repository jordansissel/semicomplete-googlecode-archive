#define _LARGEFILE64_SOURCE
#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFSIZE 4096

#define LOCKWRAP(code) { \
  pthread_mutex_lock(&offsetlock); \
  code; \
  pthread_mutex_unlock(&offsetlock); \
}


static off64_t offset = 0;
static pthread_mutex_t offsetlock = PTHREAD_MUTEX_INITIALIZER;
static int fd;

void *Reader(void *data) {
  char buf[BUFSIZE];
  int bytes = 1;

  while (bytes > 0) {
    bytes = read(fd, buf, BUFSIZE);
    LOCKWRAP( offset += bytes );
  }

  pthread_exit(NULL);
}

void *Statter(void *data) {

  off64_t seekpos;
  off64_t curoffset;
  for (;;) {
    seekpos = lseek64(fd, 0, SEEK_CUR);
    LOCKWRAP( curoffset = offset );

    if (seekpos != curoffset) {
      printf("%ld != %ld\n", seekpos, curoffset);
    }
  }

  pthread_exit(NULL);
}

int main(int argc, char **argv) {
  pthread_t reader, statter;
  char *file = "/dev/null";

  if (argc > 1)
    file = argv[1];

  fd = open(file, O_RDONLY);
  pthread_create(&reader, NULL, Reader, NULL);
  pthread_create(&statter, NULL, Statter, NULL);

  pthread_join(reader, NULL);
  pthread_join(statter, NULL);

  pthread_exit(NULL);
  return 0;
}
