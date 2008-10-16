#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "grok_input.h"
#include "grok_config.h"
#include "grok_matchconf.h"
#include "logging.h"

void conf_init(struct config *conf) {
  conf->nprograms = 0;
  conf->program_size = 10;
  conf->programs = calloc(conf->program_size, sizeof(grok_pattern_t));
}

void conf_new_program(struct config *conf) {
  conf->nprograms++;
  if (conf->nprograms == conf->program_size) {
    conf->program_size *= 2;
    conf->programs = realloc(conf->programs,
                             conf->program_size * sizeof(grok_pattern_t));
  }

  CURPROGRAM.ninputs = 0;
  CURPROGRAM.input_size = 10;
  CURPROGRAM.inputs = calloc(CURPROGRAM.input_size, sizeof(grok_input_t));

  CURPROGRAM.nmatchconfigs = 0;
  CURPROGRAM.matchconfig_size = 10;
  CURPROGRAM.matchconfigs = calloc(CURPROGRAM.matchconfig_size,
                                    sizeof(grok_matchconf_t));

  CURPROGRAM.npatternfiles = 0;
  CURPROGRAM.patternfile_size = 10;
  CURPROGRAM.patternfiles = calloc(CURPROGRAM.patternfile_size, sizeof(char *));

  //CURPROGRAM.logmask = ~0;
}

void conf_new_patternfile(struct config *conf) {
  CURPROGRAM.npatternfiles++;
  if (CURPROGRAM.npatternfiles == CURPROGRAM.patternfile_size) {
    CURPROGRAM.patternfile_size *= 2;
    CURPROGRAM.patternfiles = realloc(CURPROGRAM.patternfiles,
                                      CURPROGRAM.patternfile_size * sizeof(char *));
  }
}

void conf_new_input(struct config *conf) {
  /* Bump number of programs and grow our programs array if necessary */
  CURPROGRAM.ninputs++;
  if (CURPROGRAM.ninputs == CURPROGRAM.input_size) {
    CURPROGRAM.input_size *= 2;
    CURPROGRAM.inputs = realloc(CURPROGRAM.inputs,
                                 CURPROGRAM.input_size * sizeof(grok_input_t));
  }

  /* initialize the new input */
  memset(&CURINPUT, 0, sizeof(grok_input_t));
}

void conf_new_matchconf(struct config *conf) {
  CURPROGRAM.nmatchconfigs++;
  if (CURPROGRAM.nmatchconfigs == CURPROGRAM.matchconfig_size) {
    CURPROGRAM.matchconfig_size *= 2;
    CURPROGRAM.matchconfigs = realloc(CURPROGRAM.matchconfigs,
                                      CURPROGRAM.matchconfig_size 
                                      * sizeof(grok_matchconf_t));
  }

  grok_init(&CURMATCH.grok);
  //CURMATCH.grok.logmask = ~0;

  int i = 0;
  for (i = 0; i < CURPROGRAM.npatternfiles; i++) {
    grok_patterns_import_from_file(&CURMATCH.grok, CURPROGRAM.patternfiles[i]);
  }
}
