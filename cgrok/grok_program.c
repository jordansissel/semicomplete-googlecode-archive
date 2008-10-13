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
void _program_process_buferror(struct bufferevent *bev, short what,
                               void *data);
void _program_file_repair_event(int fd, short what, void *data);


void _program_file_read_buffer(struct bufferevent *bev, void *data);
void _program_file_read_real(int fd, short what, void *data);
//void _program_file_buferror(int fd, short what, void *data);
void _program_file_buferror(struct bufferevent *bev, short what, void *data);

void grok_program_add(grok_program_t *gprog) {
  int i = 0;

  //if (_event_init == NULL) {
    //grok_log(gprog, LOG_PROGRAM, "Calling event_init()");
    ////_event_init = event_init(); 
  //}

  for (i = 0; i < gprog->ninputs; i++) {
    printf("%d\n", i);
    gprog->inputs[i].gprog = gprog;
    grok_program_add_input(gprog, gprog->inputs + i);
  }

  grok_log(gprog, LOG_PROGRAM, "Setting up SIGCHLD: %x", gprog);
  gprog->ev_sigchld = malloc(sizeof(struct event));
  signal_set(gprog->ev_sigchld, SIGCHLD, _program_sigchld, gprog);
  signal_add(gprog->ev_sigchld, NULL);

}

void grok_program_add_input(grok_program_t *gprog, grok_input_t *ginput) {
  printf("Adding input of type %s\n",
         (ginput->type == I_FILE) ? "file" : "process");

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
  grok_input_process_t *gipt = &(ginput->source.process);
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

  bev = bufferevent_new(gipt->p_stdout, _program_process_stdout_read,
                        NULL, _program_process_buferror, ginput);
  bufferevent_enable(bev, EV_READ);

  grok_log(gprog, LOG_PROGRAM, "Scheduling start of: %s", gipt->cmd);
  event_once(-1, EV_TIMEOUT, _program_process_start, ginput, &now);
}

void grok_program_add_input_file(grok_program_t *gprog,
                                 grok_input_t *ginput) {
  struct bufferevent *bev;
  struct stat st;
  int ret;
  int pipefd[2];
  grok_input_file_t *gift = &(ginput->source.file);
  grok_log(gprog, LOG_PROGRAM, "Adding file input: %s\n", gift->filename);

  ret = stat(gift->filename, &st);
  if (ret == -1) {
    fprintf(stderr, "Failure stat(2)'ing file: %s", gift->filename);
    perror("errno says");
    return;
  }
  gift->fd = open(gift->filename, O_RDONLY);

  if (gift->fd < 0) {
    fprintf(stderr, "OMGn");
    fprintf(stderr, "Failure open(2)'ing file for read: %s\n", gift->filename);
    perror("errno says");
    return;
  }

  pipe(pipefd);
  gift->offset = 0;
  gift->reader = pipefd[0];
  gift->writer = pipefd[1];
  gift->filesize = st.st_size;
  gift->inode = st.st_ino;
  gift->waittime.tv_sec = 0;
  gift->waittime.tv_usec = 0;

  grok_log(gprog, LOG_PROGRAM, "dup2(%d, %d)", gift->fd, gift->writer);

  /* Tie our open file read fd to the writer of our pipe */
  // this doesn't work

  bev = bufferevent_new(gift->reader, _program_file_read_buffer,
                        NULL, _program_file_buferror, ginput);
  bufferevent_enable(bev, EV_READ);
  event_once(-1, EV_TIMEOUT, _program_file_read_real, ginput,
             &(gift->waittime));
}


void _program_process_stdout_read(struct bufferevent *bev, void *data) {
  grok_input_t *ginput = (grok_input_t *)data;
  char *line;
  while ((line = evbuffer_readline(EVBUFFER_INPUT(bev))) != NULL) {
    printf("(%s): '%s'\n", ginput->source.process.cmd, line);
    free(line);
  }
}

void _program_process_buferror(struct bufferevent *bev, short what,
                               void *data) {
  grok_input_t *ginput = (grok_input_t *)data;
  grok_input_process_t *gipt = &(ginput->source.process);
  grok_program_t *gprog = ginput->gprog;
  grok_log(gprog, LOG_PROGRAM, "Buffer error %d on process %d: %s",
           what, gipt->pid, gipt->cmd);
}

void _program_process_start(int fd, short what, void *data) {
  grok_input_t *ginput = (grok_input_t*)data;
  grok_input_process_t *gipt = &(ginput->source.process);
  grok_program_t *gprog = ginput->gprog;
  int pid = 0;

  /* start the process */
  pid = fork();
  if (pid != 0) {
    gipt->pid = pid;
    gipt->pgid = getpgid(pid);
    gettimeofday(&(gipt->start_time), NULL);
    grok_log(gprog, LOG_PROGRAM,
             "Starting process: '%s' (%d)", gipt->cmd, getpid());
    return;
  }
  dup2(gipt->c_stdin, 0);
  dup2(gipt->c_stdout, 1);
  execlp("sh", "sh", "-c", gipt->cmd, NULL);
  fprintf(stderr, "execlp exited unexpectedly");
  sleep(3);
  exit(-1); /* in case execlp fails */
}

void _program_file_read_buffer(struct bufferevent *bev, void *data) {
  grok_input_t *ginput = (grok_input_t *)data;
  char *line;
  while ((line = evbuffer_readline(EVBUFFER_INPUT(bev))) != NULL) {
    printf("(%s): '%s'\n", ginput->source.file.filename, line);
    free(line);
  }
}

void _program_file_buferror(struct bufferevent *bev, short what,
                            void *data) {
  grok_input_t *ginput = (grok_input_t *)data;
  grok_input_file_t *gift = &(ginput->source.file);
  grok_program_t *gprog = ginput->gprog;
  grok_log(gprog, LOG_PROGRAM, "Buffer error %d on file %d: %s",
           what, gift->fd, gift->filename);

  if (what & EVBUFFER_EOF) {
    /* EOF erro on a file, which means libevent forgets about it.
     * let's re-add it */
    event_once(0, EV_TIMEOUT, _program_file_repair_event, bev,
               &(gift->waittime));

  }
}

void _program_file_repair_event(int fd, short what, void *data) {
  struct bufferevent *bev = (struct bufferevent *)data;
  printf("Repairing event with fd %d\n", bev->ev_read.ev_fd);
  event_add(&bev->ev_read, NULL);
}

void _program_file_read_real(int fd, short what, void *data) {
  grok_input_t *ginput = (grok_input_t *)data;
  grok_input_file_t *gift = &(ginput->source.file);
  grok_program_t *gprog = ginput->gprog;

  char buffer[4096];

  int bytes = 0;
  bytes = read(gift->fd, buffer, 4096);
  write(gift->writer, buffer, bytes);
  gift->offset += bytes;

  /* we can potentially read past our last 'filesize' if the file
   * has been updated since stat()'ing it. */
  if (gift->offset > gift->filesize)
    gift->filesize = gift->offset;

  grok_log(gprog, LOG_PROGRAM, "%s: read %d bytes", gift->filename, bytes);

  if (bytes == 0) { /* nothing read, at EOF */
    /* if we're at the end of the file, figure out what to do */
    //printf("%d == %d\n", gift->offset, gift->filesize);

    struct stat st;
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
      gift->inode = st.st_ino;
    } else if (st.st_size < gift->filesize) {
      /* File size shrank */
      gift->offset = 0;
      gift->filesize = st.st_size;
      lseek(gift->fd, gift->offset, SEEK_SET);
      gift->waittime.tv_sec = 0;
    } else {
      /* Nothing changed, we should wait */
      if (gift->waittime.tv_sec == 0) {
        gift->waittime.tv_sec = 1;
      } else {
        gift->waittime.tv_sec *= 2;
        if (gift->waittime.tv_sec > 60) {
          gift->waittime.tv_sec = 60;
        }
      }
    }
  } else if (bytes < 0) { 
    printf("ERROR: Bytes read < 0: %d\n", bytes);
  } else {
    /* default wait time of 0 seconds if bytes > 0 */
    gift->waittime.tv_sec = 0;
  }


  grok_log(gprog, LOG_PROGRAM, 
           "Waiting %d seconds on %d:%s", gift->waittime.tv_sec, gift->fd,
           gift->filename);
  event_once(0, EV_TIMEOUT, _program_file_read_real, ginput,
             &(gift->waittime));
}

void _program_sigchld(int sig, short what, void *data) {
  grok_program_t *gprog = (grok_program_t*)data;
  int i = 0;
  int pid, status;
  grok_log(gprog, LOG_PROGRAM, "sigchld handler");

  while ( (pid = waitpid(-1, &status, WNOHANG)) > 0) {
    /* we found a dead child. Look for an input_process it belongs to,
     * then see if we should restart it
     */
    for (i = 0; i < gprog->ninputs; i++) {
      grok_input_t *ginput = (gprog->inputs + i);
      if (ginput->type != I_PROCESS)
        continue;
        
      grok_input_process_t *gipt = &(ginput->source.process);
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

        grok_log(gprog, LOG_PROGRAM, 
                 "Scheduling process restart in %d.%d seconds: %s\n",
                 restart_delay.tv_sec, restart_delay.tv_usec, gipt->cmd);
        event_once(-1, EV_TIMEOUT, _program_process_start,
                   ginput, &restart_delay);

      }
    } /* end for */
  } /* end while */
}

void grok_program_loop(void) {
  event_dispatch();
}

int main(int argc, char **argv) {
  grok_program_t *gprog;
  struct event_base *ebase;
  int i = 0;

  gprog = calloc(1, sizeof(grok_input_t));
  gprog->logmask = ~0;
  gprog->inputs = calloc(10, sizeof(grok_input_t));

  //gprog->inputs[i].type = I_PROCESS;
  //gprog->inputs[i].source.process.cmd = "tail -0f /var/log/messages";
  //gprog->inputs[i].source.process.restart_on_death = 1;

  gprog->inputs[i].type = I_FILE;
  //gprog->inputs[i].source.file.filename = "/var/log/messages";
  gprog->inputs[i].source.file.filename = "/tmp/test";

  gprog->inputs[++i].type = I_PROCESS;
  gprog->inputs[i].source.process.cmd = "uptime";
  gprog->inputs[i].source.process.run_interval = 5;
  gprog->inputs[i].source.process.min_restart_delay = 10;

  gprog->ninputs = i+1;

  ebase = event_init();
  grok_program_add(gprog);
  grok_program_loop();
  free(gprog->inputs);
  free(gprog);
  event_base_free(ebase);
  return 0;
}
