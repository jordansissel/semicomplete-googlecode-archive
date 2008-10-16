#include "grok_program.h"

#define CURPROGRAM (conf->programs[conf->nprograms])
#define CURINPUT (CURPROGRAM.inputs [CURPROGRAM.ninputs] )
#define CURMATCH (CURPROGRAM.matchconfigs[CURPROGRAM.nmatchconfigs])

struct config {
  grok_program_t *programs;
  int nprograms;
  int program_size;
};

void conf_new_program(struct config *conf);
void conf_new_input(struct config *conf);
