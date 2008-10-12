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

int main() {
  struct bufferevent *bev;
  struct event ev;
  int fd;

  int c[2];
  pipe(c);
  event_init();

  /* have a bufferevent watch our child's stdout */
  bev = bufferevent_new(c[0], bufread, NULL, NULL, NULL);
  bufferevent_enable(bev, EV_READ);
  //struct event ev;

  //fd = open("/var/log/messages", O_RDONLY);
  //timeoutk
  write(c[1], "Hello there\n", 12);

  //lseek(fd, 0, SEEK_SET);
  event_dispatch();

  return 0;
}
