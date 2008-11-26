#include "grok.h"
#include "grok_matchconf.h"
#include "logging.h"
#include "libc_helper.h"

/* for strndup(3) */
#define _GNU_SOURCE

static int mcgrok_init = 0;
static grok_t matchconfig_grok;

void grok_matchconfig_init(grok_program_t *gprog, grok_matchconf_t *gmc) {
  grok_init(&gmc->grok);
  gmc->shell = NULL;
  gmc->reaction = NULL;
  gmc->shellinput = NULL;

  if (mcgrok_init == 0) {
    grok_init(&matchconfig_grok);
    grok_patterns_import_from_string(&matchconfig_grok, "PATTERN \\%\\{%{NAME}\\}");
    grok_patterns_import_from_string(&matchconfig_grok, "NAME \\w+(?::\\w+)?");
    grok_compile(&matchconfig_grok, "%{PATTERN}");
    mcgrok_init = 1;
  }
}

void grok_matchconfig_global_cleanup(void) {
  if (mcgrok_init) {
    grok_free(&matchconfig_grok);
  }
}

void grok_matchconfig_close(grok_program_t *gprog, grok_matchconf_t  *gmc) {
  int ret = 0;
  if (gmc->shellinput != NULL) {
    ret = fclose(gmc->shellinput);
    grok_log(gprog, LOG_PROGRAM, "Closing matchconf shell. fclose() = %d", ret);
    gmc->shellinput = NULL;
  }
  grok_free(&gmc->grok);
}
void grok_matchconfig_exec(grok_program_t *gprog, grok_input_t *ginput,
                           const char *text) {
  grok_t *grok;
  grok_match_t gm;
  grok_matchconf_t *gmc;
  int i = 0;

  for (i = 0; i < gprog->nmatchconfigs; i++) {
    int ret;
    gmc = &gprog->matchconfigs[i];
    grok = &gmc->grok;
    if (gmc->is_nomatch) {
      continue;
    }

    grok_log(gprog, LOG_PROGRAM, "Trying match against : %s",
             gmc->reaction);
    ret = grok_exec(grok, text, &gm);
    if (ret >= 0) {
      grok_matchconfig_react(gprog, ginput, gmc, &gm);
    }
  }
}

void grok_matchconfig_react(grok_program_t *gprog, grok_input_t *ginput, 
                            grok_matchconf_t *gmc, grok_match_t *gm) {
  char *reaction;
  ginput->instance_match_count++;
  reaction = grok_matchconfig_filter_reaction(gmc->reaction, gm);
  if (reaction == NULL) {
    reaction = gmc->reaction;
  }

  if (gmc->shellinput == NULL) {
    grok_matchconfig_start_shell(gprog, gmc);
  }

  grok_log(gprog, LOG_PROGRAM, "Sending '%s' to subshell", reaction);
  fprintf(gmc->shellinput, "%s\n", reaction);
  if (gmc->flush) {
    grok_log(gprog, LOG_PROGRAM, "flush enabled, calling fflush");
    fflush(gmc->shellinput);
  }

  if (reaction != gmc->reaction) {
    free(reaction);
  }
}

void grok_matchconfig_exec_nomatch(grok_program_t *gprog, grok_input_t *ginput) {
  int i = 0;
  for (i = 0; i < gprog->nmatchconfigs; i++) {
    grok_matchconf_t *gmc = &gprog->matchconfigs[i];
    if (gmc->is_nomatch) {
      grok_log(gprog, LOG_PROGRAM, "Executing reaction for nomatch: %s",
               gmc->reaction);
      grok_matchconfig_react(gprog, ginput, gmc, NULL);
    }
  }
}

char *grok_matchconfig_filter_reaction(const char *str, grok_match_t *gm) {
  char *output;
  int len;
  int size;
  grok_match_t tmp_gm;
  int offset = 0;

  if (gm == NULL) {
    return NULL;
  }

  len = strlen(str);
  size = len + 1;
  output = malloc(size);
  memcpy(output, str, size);

  grok_log(gm->grok, LOG_PROGRAM, "Checking '%.*s'", len - offset, output + offset);
  matchconfig_grok.logmask = gm->grok->logmask;
  matchconfig_grok.logdepth  = gm->grok->logdepth + 1;
  while (grok_execn(&matchconfig_grok, output + offset,
                    len - offset, &tmp_gm) >= 0) {
    grok_log(gm->grok, LOG_PROGRAM, "Checking '%.*s'",
             len - offset, output + offset);
    char *name = NULL, *value = NULL, *name_copy;
    int name_len, value_len;
    int ret = -1;

    grok_match_get_named_substring(&tmp_gm, "NAME", 
                                   (const char **)&name, &name_len);
    grok_log(gm->grok, LOG_PROGRAM, "Matched something: %.*s", name_len, name);

    /* XXX: If this list of names gets any larger, we should use gperf to avoid
     * extra string comparisons */
    if (!strncmp(name, "_LINE", name_len)) {
      value = gm->subject;
      value_len = strlen(value);
      ret = 0;
    } else if (!strncmp(name, "_MATCH", name_len)) {
      value = gm->subject + gm->start;
      value_len = gm->end - gm->start;
      ret = 0;
    } else {
      /* XXX: Should just have get_named_substring take a 
       * 'name, name_len' instead */
      name_copy = malloc(name_len + 1);
      memcpy(name_copy, name, name_len);
      name_copy[name_len] = '\0';
      ret = grok_match_get_named_substring(gm, name_copy, (const char **)&value,
                                           &value_len);
      free(name_copy);
    }

    if (ret != 0) {
      offset += tmp_gm.end;
    } else {
      /* replace %{FOO} with the value of foo */
      grok_log(tmp_gm.grok, LOG_PROGRAM, "Start/end: %d %d", tmp_gm.start, tmp_gm.end);
      grok_log(tmp_gm.grok, LOG_PROGRAM, "Replacing %.*s with %.*s",
               (tmp_gm.end - tmp_gm.start),
               output + tmp_gm.start + offset, value_len, value);
      substr_replace(&output, &len, &size, offset + tmp_gm.start,
                     offset + tmp_gm.end, value, value_len);
      offset += value_len;
    }
  }

  return output;
}

void grok_matchconfig_start_shell(grok_program_t *gprog,
                                  grok_matchconf_t *gmc) {
  grok_collection_t *gcol = gprog->gcol;
  int pipefd[2];
  int pid;

  safe_pipe(pipefd);
  grok_log(gprog, LOG_PROGRAM, "Starting matchconfig subshell: %s\n",
           (gmc->shell == NULL) ? "/bin/sh" : gmc->shell);
  gmc->pid = fork();
  if (gmc->pid == 0) {
    close(pipefd[1]); /* close the stdin input from the parent */
    /* child */
    dup2(pipefd[0], 0);
    if (gmc->shell == NULL) { 
      /* default to just sh if there's no shell set */
      execlp("sh", "sh", NULL);
    } else {
      execlp("sh", "sh", "-c", gmc->shell, NULL);
    }
    fprintf(stderr, "!!! Shouldn't have gotten here. execlp failed");
    perror("errno says");
    exit(-1);;
  }
  gmc->shellinput = fdopen(pipefd[1], "w");
  if (gmc->shellinput == NULL) {
    fprintf(stderr, "FATAL: Unable to fdopen(%d)\n", pipefd[1]);
    perror("errno says");
    exit(1); /* XXX: Don't exit, it's mean */
  }
}
