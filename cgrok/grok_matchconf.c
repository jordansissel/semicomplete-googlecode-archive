#include "grok.h"
#include "grok_matchconf.h"
#include "logging.h"

void grok_matchconfig_exec(grok_program_t *gprog, grok_matchconf_t *gmc,
                           const char *text) {
  grok_t *grok;
  grok_match_t gm;
  grok = &gmc->grok;
  int ret;
  ret = grok_exec(grok, text, &gm);
  if (ret >= 0) { 
    char *name;
    const char *data;
    int namelen, datalen;
    void *handle;
    handle = grok_match_walk_init(&gm);
    while (grok_match_walk_next(&gm, handle, &name, &namelen, &data, &datalen)
           == 0) { 
      printf("%.*s => %.*s\n", namelen, name, datalen, data);
      free(name);
    };
    grok_match_walk_end(&gm, handle);

  } 
}

