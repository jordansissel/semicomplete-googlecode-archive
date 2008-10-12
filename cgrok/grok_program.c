#include "grok.h"
#include "grok_program.h"

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <event.h>
#include <signal.h>
#include <fcntl.h>

static void *_event_init = NULL;
void _program_sigchld(int sig, short what, void *data);

void _program_process_stdout_read(struct bufferevent *bev, void *data);
void _program_process_start(int fd, short what, void *data);

void _program_file_read_buffer(struct bufferevent *bev, void *data);
void _program_file_read_real(int fd, short what, void *data);

void grok_program_add(grok_program_t *gprog) {
  int i = 0;
  struct event sigevent;

  if (_event_init == NULL)
    _event_init = event_init();

  signal_set(&sigevent, SIGCHLD, _program_sigchld, gprog);
  signal_add(&sigevent, NULL);

  for (i = 0; i < gprog->ninputs; i++) {
    gprog->inputs[i].gprog = gprog;
    grok_program_add_input(gprog, gprog->inputs + i);
  }
}

void grok_program_add_input(grok_program_t *gprog, grok_input_t *ginput) {
  switch (ginput->type) {
    case I_FILE:
      grok_program_add_input_file(gprog, ginput);
      break;
    case I_PROCESS:
      grok_program_add_input_process(gprog, ginput);
      break;
  }
}

void grok_program_add_input_process(grok_program_t *gprog,
                                    grok_input_t *ginput) {
  struct bufferevent *bev;
  grok_input_process_t *gipt = ginput->source.process;
  int childin[2], childout[2], childerr[2];
  int pid;
  struct timeval now = { 0, 0 };

  pipe(childin);
  pipe(childout);
  pipe(childerr);

  gipt->p_stdin = childin[1];
  gipt->p_stdout = childout[0];
  gipt->p_stderr = childerr[0];
  gipt->c_stdin = childin[0];
  gipt->c_stdout = childout[1];
  gipt->c_stderr = childerr[1];

  bev = bufferevent_new(gipt->p_stdout,
                        _program_process_stdout_read,
                        NULL, NULL, ginput);
  bufferevent_enable(bev, EV_READ);

  event_once(0, EV_TIMEOUT, _program_process_start, ginput, &now);
}

void grok_program_add_input_file(grok_program_t *gprog,
                                 grok_input_t *ginput) {
  struct bufferevent *bev;
  struct stat st;
  int ret;
  int pipefd[2];
  grok_input_file_t *gift = ginput->source.file;

  ret = stat(gift->filename, &st);
  if (ret == -1) {
    fprintf(stderr, "Failure stat(2)'ing file: %s\n", gift->filename);
    perror("errno says");
    return;
  }

  gift->fd = open(gift->filename, O_RDONLY);

  if (gift->writer < 0) {
    fprintf(stderr, "Failure open(2)'ing file for read: %s\n", gift->filename);
    perror("errno says");
    return;
  }

  pipe(pipefd);
  gift->offset = 0;
  gift->reader = pipefd[0];
  gift->writer = pipefd[1];
  //gift->filesize = st.st_blksize * st.st_blocks;
  gift->filesize = st.st_size;
  gift->inode = st.st_ino;
  gift->waittime.tv_sec = 0;
  gift->waittime.tv_usec = 0;

  /* Tie our open file read fd to the writer of our pipe */
  // this doesn't work
  //dup2(gift->fd, gift->writer);

  bev = bufferevent_new(gift->reader, _program_file_read_buffer, NULL, NULL,
                        ginput);
  bufferevent_enable(bev, EV_READ);

  event_once(0, EV_TIMEOUT, _program_file_read_real, ginput,
             &(gift->waittime));
}


void _program_process_stdout_read(struct bufferevent *bev, void *data) {
  grok_input_t *ginput = (grok_input_t *)data;
  char *line;
  while ((line = evbuffer_readline(EVBUFFER_INPUT(bev))) != NULL) {
    printf("Line: '%s'\n", line);
    free(line);
  }
}

void _program_process_start(int fd, short what, void *data) {
  grok_input_t *ginput = (grok_input_t*)data;
  grok_input_process_t *gipt = ginput->source.process;
  int pid;

  /* start the process */
  pid = fork();
  if (pid != 0) {
    gipt->pid = pid;
    gipt->pgid = getpgid(pid);
    gettimeofday(&(gipt->start_time), NULL);
    return;
  }

  fprintf(stderr, "Starting process: '%s' (%d)\n", gipt->cmd, getpid());
  dup2(gipt->c_stdin, 0);
  dup2(gipt->c_stdout, 1);
  execlp("sh", "sh", "-c", gipt->cmd, NULL);
  fprintf(stderr, "execlp exited unexpectedly");
  exit(-1); /* in case execlp fails */
}

void _program_file_read_buffer(struct bufferevent *bev, void *data) {
  grok_input_t *ginput = (grok_input_t *)data;
  char *line;
  if (EVBUFFER_INPUT(bev) == NULL) {
    printf("WTF?!?!?");
    return;
  }
  while ((line = evbuffer_readline(EVBUFFER_INPUT(bev))) != NULL) {
    printf("FileLine: '%s'\n", line);
    free(line);
  }
}

void _program_file_read_real(int fd, short what, void *data) {
  grok_input_t *ginput = (grok_input_t *)data;
  grok_input_file_t *gift = ginput->source.file;

  char buffer[4096];

  //printf("file: off %d vs size %d\n", gift->offset, gift->filesize);
  if (gift->offset == gift->filesize) {
    /* if we're at the end of the file, figure out what to do */
    //printf("%d == %d\n", gift->offset, gift->filesize);

    struct stat st;
    //if (fstat(gift->fd, &st) == 0) {
    //} else 
    if (stat(gift->filename, &st) == 0) {
      //perror("fstat failed\n");
    } else {
      perror("stat failed\n");
    }

    if (gift->inode != st.st_ino) {
      /* inode changed, reopen file */
      printf("file inode changed from %d to %d\n", gift->inode, st.st_ino);
      close(gift->fd);
      gift->fd = open(gift->filename, O_RDONLY);
      gift->waittime.tv_sec = 0;
      gift->offset = 0;
    } else if (st.st_size < gift->filesize) {
      /* File size changed */
      //printf("file size shrank from %d to %d\n", gift->filesize, st.st_size);
      gift->offset = 0;
      lseek(gift->fd, gift->offset, SEEK_SET);
      gift->waittime.tv_sec = 0;
    } else if (st.st_size > gift->filesize) {
      //printf("file size grew from %d to %d\n", gift->filesize, st.st_size);
      //lseek(gift->fd, gift->offset, SEEK_SET);
      gift->waittime.tv_sec = 0;
    } else {
      /* Nothing changed, we should wait */
      if (gift->waittime.tv_sec == 0) {
        gift->waittime.tv_sec = 1;
      } else {
        gift->waittime.tv_sec *= 2;
        if (gift->waittime.tv_sec > 60)
          gift->waittime.tv_sec = 60;
      }
    }
    gift->filesize = st.st_size;
    gift->inode = st.st_ino;
  } else {
    int bytes = 0;
    bytes = read(gift->fd, buffer, 4096);
    write(gift->writer, buffer, bytes);
    gift->offset += bytes;

    /* we can potentially read past our last 'filesize' if the file
     * has been updated since stat()'ing it. */
    if (gift->offset > gift->filesize)
      gift->filesize = gift->offset;

    /* read successful means we should reset the clock */
    gift->waittime.tv_sec = 0;
  }

  //printf("Waiting %d seconds on %d:%s\n", gift->waittime.tv_sec, gift->fd,
         //gift->filename);
  event_once(0, EV_TIMEOUT, _program_file_read_real, ginput,
             &(gift->waittime));
}

void _program_sigchld(int sig, short what, void *data) {
  printf("fester\n");
  grok_program_t *gprog = (grok_program_t*)data;
  int i = 0;
  int pid, status;


  while ( (pid = waitpid(-1, &status, WNOHANG)) > 0) {
    /* we found a dead child. Look for an input_process it belongs to,
     * then see if we should restart it
     */
    for (i = 0; i < gprog->ninputs; i++) {
      grok_input_t *ginput = (gprog->inputs + i);
      printf("Checking ginput %x\n", ginput);
      if (ginput->type != I_PROCESS)
        continue;
        
      grok_input_process_t *gipt = ginput->source.process;
      if (gipt->pid != pid)
        continue;

      if (PROCESS_SHOULD_RESTART(gipt)) {
        struct timeval restart_delay = { 0, 0 };
        if (gipt->run_interval > 0) {
          struct timeval interval = { gipt->run_interval, 0 };
          struct timeval duration;
          struct timeval now;
          gettimeofday(&now, NULL);
          timersub(&now, &(gipt->start_time), &duration);
          timersub(&interval, &duration, &restart_delay);
        }

        if (gipt->min_restart_delay > 0) {
          struct timeval fixed_delay = { gipt->min_restart_delay, 0 };

          if (timercmp(&restart_delay, &fixed_delay, <)) {
            restart_delay.tv_sec = fixed_delay.tv_sec;
            restart_delay.tv_usec = fixed_delay.tv_usec;
          }
        }

        fprintf(stderr, "Scheduling process restart in %d.%d seconds: %s\n",
                restart_delay.tv_sec, restart_delay.tv_usec, gipt->cmd);
        event_once(0, EV_TIMEOUT, _program_process_start,
                   ginput, &restart_delay);

      }
    } /* end for */
  } /* end while */
}

void grok_program_loop(void) {
  event_dispatch();
}

int main(int argc, char **argv) {
  grok_program_t gprog;
  grok_input_process_t *gipt;
  grok_input_file_t *gift;

  event_init();
  gprog.ninputs = 2;
  gprog.inputs = calloc(gprog.ninputs, sizeof(grok_input_t));
  gprog.inputs[0].type = I_PROCESS;
  gipt = calloc(1, sizeof(grok_input_process_t));
  //gipt->cmd = "tail -0f /var/log/messages";
  gipt->cmd = "uptime";
  gipt->restart_on_death = 1;
  gprog.inputs[0].source.process = gipt;

  //gprog.inputs[1].type = I_PROCESS;
  //gipt = calloc(1, sizeof(grok_input_process_t));
  //gipt->cmd = "uptime";
  //gipt->run_interval = 5;
  //gipt->min_restart_delay = 5;
  //gprog.inputs[1].source.process = gipt;

  gprog.inputs[1].type = I_FILE;
  gift = calloc(1, sizeof(grok_input_file_t));
  gift->filename = "/tmp/test";
  gift->follow = 1;
  gprog.inputs[1].source.file = gift;
  //gprog.inputs[1].type = I_FILE;
  //gprog.inputs[1].source.file.filename = "/var/log/messages";
  //gprog.inputs[1].source.file.follow = 1;

  grok_program_add(&gprog);
  grok_program_loop();
  return 0;
}

