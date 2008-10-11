#include "grok.h"
#include "grok_program.h"

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <event.h>
#include <signal.h>

static void *_event_init = NULL;
void _program_process_stdout_read(struct bufferevent *bev, void *data);
void _program_process_start(int fd, short what, void *data);
void _program_sigchld(int sig, short what, void *data);

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

  bev = bufferevent_new(gipt->p_stdout,
                        _program_process_stdout_read,
                        NULL, NULL, ginput);
  bufferevent_enable(bev, EV_READ);

  event_once(0, EV_TIMEOUT, _program_process_start, ginput, &now);
}

int main(int argc, char **argv) {
  grok_program_t gprog;
  grok_input_process_t tailf;

  event_init();
  gprog.ninputs = 2;
  gprog.inputs = calloc(gprog.ninputs, sizeof(grok_input_t));
  gprog.inputs[0].type = I_PROCESS;
  gprog.inputs[0].source.process.cmd = "tail -0f /var/log/messages";
  gprog.inputs[0].source.process.restart_on_death = 1;
  gprog.inputs[1].type = I_PROCESS;
  gprog.inputs[1].source.process.cmd = "uptime";
  gprog.inputs[1].source.process.run_interval = 5;
  gprog.inputs[1].source.process.min_restart_delay = 10;

  grok_program_add(&gprog);
  grok_program_loop();
  return 0;
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
  grok_input_process_t *gipt = &(ginput->source.process);
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

void _program_sigchld(int sig, short what, void *data) {
  grok_program_t *gprog = (grok_program_t*)data;
  int i = 0;
  int pid, status;

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

