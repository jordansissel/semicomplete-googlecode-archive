#include "grok.h"
#include "grok_program.h"
#include "grok_config.h"

#include "conf.tab.h"

extern FILE *yyin; /* from conf.lex (flex provided) */

int main() {
  struct config c;
  grok_collection_t *gcol;
  int i;

  yyin = fopen("grok.conf.test", "r");
  conf_init(&c);
  yyparse(&c);

  gcol = grok_collection_init();
  for (i = 0; i <= c.nprograms; i++) {
    printf("Adding program %d\n", i);
    grok_collection_add(gcol, &(c.programs[i]));
  }
  grok_collection_loop(gcol);

  return 0;
}
