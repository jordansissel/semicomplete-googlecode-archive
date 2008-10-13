#ifndef _GROK_PROGRAM_H_
#define _GROK_PROGRAM_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <event.h>

//include "grok_input.h"
//include "grok_matchconf.h"

typedef struct grok_program grok_program_t;
struct grok_input;
struct grok_matchconfig;

struct grok_program {
  struct grok_input *inputs;
  int ninputs;
  int logmask;
  int logdepth;
  struct grok_matchconf *matchconfigs;
  int nmatchconfigs;
  struct event *ev_sigchld;
};

void grok_program_add(grok_program_t *gprog);
void grok_program_loop(void);

#endif /* _GROK_PROGRAM_H_ */
