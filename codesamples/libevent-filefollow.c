#include <sys/types.h>
#include <sys/stat.h>
#include <sys/queue.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <event.h>

void bufread(struct bufferevent *bev, void *data) {
  char *line;
  while ( (line = evbuffer_readline(bev->input)) != NULL ) {
    printf("Line: '%s'\n", line);
    free(line);
  }
}

void fileread(int fd, short what, void *data) {
  printf("fd %d / %d ready\n", fd, what);
}

void err(struct bufferevent *bev, short what, void *data) {
  printf("err: %d\n", what);
}

int main() {
  struct bufferevent *bev;
  int fd;

  event_init();

  /* have a bufferevent watch our child's stdout */
  //bev = bufferevent_new(fd, bufread, NULL, err, NULL);
  //bufferevent_enable(bev, EV_READ);
  struct event ev;
  fd = open("/var/log/messages", O_RDONLY);
  event_set(&ev, fd, EV_READ, fileread, NULL);
  struct timeval t = {3, 0};
  event_add(&ev, &t);

  //lseek(fd, 0, SEEK_SET);
  event_dispatch();

  return 0;
}
