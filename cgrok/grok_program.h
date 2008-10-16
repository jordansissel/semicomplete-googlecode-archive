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
  char *name; /* optional program name */

  struct grok_input *inputs;
  int ninputs;
  int input_size;

  struct grok_matchconf *matchconfigs;
  int nmatchconfigs;
  int matchconfig_size;

  int logmask;
  int logdepth;
  struct event *ev_sigchld;
};

void grok_program_add(grok_program_t *gprog);
void grok_program_loop(void);

#endif /* _GROK_PROGRAM_H_ */
