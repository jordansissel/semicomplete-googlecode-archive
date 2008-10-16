#ifndef _GROK_MATCHCONF_H_
#define _GROK_MATCHCONF_H_

#include "grok.h"
#include "grok_program.h"
#include "grok_matchconf.h"

typedef struct grok_matchconf grok_matchconf_t;
typedef struct grok_reaction grok_reaction_t;

struct grok_matchconf {
  grok_t grok; /* The grok pattern to match */
};

struct grok_reaction {
  char *cmd;
};

void grok_matchconfig_exec(grok_program_t *gprog, grok_matchconf_t *gmc, const char *text);

#endif /*  _GROK_MATCHCONF_H_ */
