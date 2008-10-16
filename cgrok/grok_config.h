#include "grok_program.h"

#define CURPROGRAM (conf->programs[conf->nprograms - 1])
#define CURINPUT (CURPROGRAM.inputs [CURPROGRAM.ninputs - 1])
#define CURMATCH (CURPROGRAM.matchconfigs [CURPROGRAM.nmatchconfigs - 1])
#define CURPATTERNFILE (CURPROGRAM.patternfiles [CURPROGRAM.npatternfiles - 1])

struct config {
  grok_program_t *programs;
  int nprograms;
  int program_size;
};

void conf_new_program(struct config *conf);
void conf_new_input(struct config *conf);
