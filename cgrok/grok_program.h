#include <sys/types.h>
#include <sys/stat.h>
#include <event.h>

#include "grok_input.h"

typedef struct grok_program grok_program_t;
typedef struct grok_matchconf grok_matchconf_t;

struct grok_program {
  grok_input_t *inputs;
  int ninputs;
  int logmask;
  int logdepth;
  grok_matchconf_t *matchconfigs;
  int nmatchconfigs;
  struct event *ev_sigchld;
};

void grok_program_add(grok_program_t *gprog);
void grok_program_loop(void);
