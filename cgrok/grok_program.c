#include "grok.h"
#include "grok_program.h"
#include "grok_input.h"
#include "grok_matchconf.h"

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
  gprog->inputs[i].source.file.filename = "/var/log/messages";

  gprog->inputs[++i].type = I_PROCESS;
  gprog->inputs[i].source.process.cmd = "ifconfig";
  gprog->inputs[i].source.process.run_interval = 5;
  gprog->inputs[i].source.process.min_restart_delay = 10;

  gprog->ninputs = i+1;

  i = 0;
  gprog->matchconfigs = calloc(10, sizeof(grok_matchconf_t));
  grok_t *grok = &(gprog->matchconfigs[i].grok);
  grok_init(grok);
  grok_patterns_import_from_file(grok, "./pcregrok_patterns");
  grok_compile(grok, "%{IP}");
  gprog->nmatchconfigs = i + 1;

  ebase = event_init();
  grok_program_add(gprog);
  grok_program_loop();
  free(gprog->inputs);
  free(gprog);
  event_base_free(ebase);
  return 0;
}
