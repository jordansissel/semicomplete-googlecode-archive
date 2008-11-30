/* for strndup(3) */
#define _GNU_SOURCE

#include <errno.h>
#include <string.h>
#include "grok.h"
#include "grok_matchconf.h"
#include "grok_matchconf_macro.h"
#include "logging.h"
#include "libc_helper.h"
#include "filters.h"

char *grok_match_reaction_apply_filter(grok_match_t *gm,
                                       char **value, int *value_len,
                                       const char *filter, int filter_len);

static int mcgrok_init = 0;
static grok_t matchconfig_grok;

void grok_matchconfig_init(grok_program_t *gprog, grok_matchconf_t *gmc) {
  grok_init(&gmc->grok);
  gmc->shell = NULL;
  gmc->reaction = NULL;
  gmc->shellinput = NULL;

  if (mcgrok_init == 0) {
    grok_init(&matchconfig_grok);
    grok_patterns_import_from_string(&matchconfig_grok, 
                                     "PATTERN \\%\\{%{NAME}(?:%{FILTER})?}");
    grok_patterns_import_from_string(&matchconfig_grok, "NAME @?\\w+(?::\\w+)?(?:|\\w+)*");
    grok_patterns_import_from_string(&matchconfig_grok, "FILTER (?:\\|\\w+)+");
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

  /* This clause will occur if grok_matchconfig_filter_reaction had to do
   * any meaningful work replacing %{FOO} and such */
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

  grok_log(gm->grok, LOG_REACTION,
           "Checking '%.*s'", len - offset, output + offset);
  matchconfig_grok.logmask = gm->grok->logmask;
  matchconfig_grok.logdepth  = gm->grok->logdepth + 1;
  while (grok_execn(&matchconfig_grok, output + offset,
                    len - offset, &tmp_gm) >= 0) {
    grok_log(gm->grok, LOG_REACTION, "Checking '%.*s'",
             len - offset, output + offset);
    const char *name = NULL;
    const char *filter = NULL;
    char *value = NULL;
    char *name_copy;

    int name_len, value_len, filter_len;
    int ret = -1;
    int free_value = 0;
    const struct strmacro *patmacro;

    grok_match_get_named_substring(&tmp_gm, "NAME", &name, &name_len);
    grok_match_get_named_substring(&tmp_gm, "FILTER", &filter, &filter_len);
    grok_log(gm->grok, LOG_REACTION, "Matched something: %.*s", name_len, name);

    /* XXX: We should really make a dispatch table out of this... */
    /* _macro_dispatch_func(char **value, int *value_len) ... */
    /* Let gperf do the hard work for us. */
    patmacro = patname2macro(name, name_len);
    grok_log(gm->grok, LOG_REACTION, "Checking lookup table for '%.*s': %x",
             name_len, name, patmacro);
    if (patmacro != NULL) {
      free_value = 1; /* We malloc stuff to 'value' here */
      switch (patmacro->code) {
        case VALUE_LINE:
          value = strdup(gm->subject);
          value_len = strlen(value);
          ret = 0;
          break;
        case VALUE_START:
          value_len = asprintf(&value, "%d", gm->start);
          ret = 0;
          break;
        case VALUE_END:
          value_len = asprintf(&value, "%d", gm->end);
          ret = 0;
          break;
        case VALUE_LENGTH:
          value_len = asprintf(&value, "%d", gm->end - gm->start);
          ret = 0;
          break;
        case VALUE_MATCH:
          value_len = gm->end - gm->start;
          value = strndup(gm->subject + gm->start, value_len);
          ret = 0;
          break;
        case VALUE_JSON:
          { 
            void *handle;
            int value_offset = 0;
            int value_size = 0;
            char *pname;
            const char *pdata;
            int pname_len, pdata_len;

            value = NULL;
            value_len = 0;

            /* Push @FOO values first */
            substr_replace(&value, &value_len, &value_size, value_len, value_len,
                           "\"@LINE\": \"%{@LINE|jsonencode}\", ", 32);
            substr_replace(&value, &value_len, &value_size, value_len, value_len,
                           "\"@MATCH\": \"%{@MATCH|jsonencode}\", ", 34);

            /* Don't quote the values here since they're numbers */
            substr_replace(&value, &value_len, &value_size, value_len, value_len,
                           "\"@START\": %{@START}, ", 21);
            substr_replace(&value, &value_len, &value_size, value_len, value_len,
                           "\"@END\": %{@END}, ", 17);
            value_offset += value_len;

            /* For every named capture, put this in our result string:
             * "NAME": "%{NAME|jsonencode}"
             */
            handle = grok_match_walk_init(gm);
            while (grok_match_walk_next(gm, handle, &pname, &pname_len,
                                        &pdata, &pdata_len) == 0) {
              substr_replace(&value, &value_len, &value_size,
                             value_offset, -1,
                             "\"\": \"%{|jsonencode}\", ", 22);
              substr_replace(&value, &value_len, &value_size,
                             value_offset + 1, -1,
                             pname, pname_len);
              value_offset += pname_len;

              substr_replace(&value, &value_len, &value_size,
                             value_offset + 7, -1,
                             pname, pname_len);
              value_offset += pname_len;
              value_offset += 22; /* strlen: '"": "%{|jsonencode}", ' */
              free(pname); /* alloc'd by grok_match_walk_next */
            }
            grok_match_walk_end(gm, handle);

            /* Insert the { at the beginning */
            substr_replace(&value, &value_len, &value_size, 0, 0, "{ ", 2);

            /* Replace trailing ", " with " }" */
            substr_replace(&value, &value_len, &value_size, value_offset,
                           value_offset + 1, " }", 2);

            char *old = value;
            grok_log(gm->grok, LOG_REACTION, "JSON intermediate: %.*s",
                     value_len, value);
            value = grok_matchconfig_filter_reaction(value, gm);
            free(old);

            ret = 0;
          }
          break;
        default:
          grok_log(gm->grok, LOG_REACTION, "Unhandled macro code: '%.*s' (%d)",
                   name_len, name, patmacro->code);
      }
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
      char *old;
      grok_log(tmp_gm.grok, LOG_REACTION, "Start/end: %d %d", tmp_gm.start, tmp_gm.end);
      grok_log(tmp_gm.grok, LOG_REACTION, "Replacing %.*s",
               (tmp_gm.end - tmp_gm.start), output + tmp_gm.start + offset);

      /* apply the any filters from %{FOO|filter1|filter2...} */
      old = value;
      grok_match_reaction_apply_filter(gm, &value, &value_len,
                                       filter, filter_len);
      if (old != value) {
        if (free_value) {
          free(old); /* Free the old value */
        }
        free_value = 1;
      }
      grok_log(gm->grok, LOG_REACTION, "Filter: %.*s", filter_len, filter);

      grok_log(tmp_gm.grok, LOG_REACTION, "Replacing %.*s with %.*s",
               (tmp_gm.end - tmp_gm.start),
               output + tmp_gm.start + offset, value_len, value);
      substr_replace(&output, &len, &size, offset + tmp_gm.start,
                     offset + tmp_gm.end, value, value_len);
      offset += value_len;
      if (free_value) {
        free(value);
      }
    }
  }

  return output;
}

void grok_matchconfig_start_shell(grok_program_t *gprog,
                                  grok_matchconf_t *gmc) {
  grok_collection_t *gcol = gprog->gcol;
  int pipefd[2];
  int pid;

  if (!strcmp(gmc->shell, "stdout")) {
    /* Special case: Use the stdout FILE */
    grok_log(gprog, LOG_PROGRAM, 
             "matchconfig subshell set to 'stdout', directing reaction " \
             "output to stdout instead of a process.");
    gmc->shellinput = stdout;
    return;
  } 
  safe_pipe(pipefd);
  grok_log(gprog, LOG_PROGRAM, "Starting matchconfig subshell: %s",
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
    grok_log(gprog, LOG_PROGRAM, "Fatal: Unable to fdopen(%d) subshell pipe: %s",
             pipefd[1], strerror(errno));
    exit(1); /* XXX: We shouldn't exit here, but what else should we do? */
  }
}

char *grok_match_reaction_apply_filter(grok_match_t *gm,
                                       char **value, int *value_len,
                                       const char *filter, int filter_len) {
  int offset = 0, len = 0;
  int value_size;
  int ret;
  struct filter *filterobj;

  if (filter_len == 0) {
    return *value;
  }

  *value = strndup(*value, *value_len);
  /* we'll use the value_len from the function arguments */
  value_size = *value_len + 1;

  /* skip first char which must be a '|' */
  offset = 1;

  while (offset + len < filter_len) {
    if (filter[offset + len] == '|') {
      /* Apply the filter */
      grok_log(gm->grok, LOG_REACTION, "ApplyFilter code: %.*s",
               len, filter + offset);
      filterobj = string_filter_lookup(filter + offset, len);
      if (filterobj == NULL) {
        grok_log(gm->grok, LOG_REACTION, 
                 "Can't apply filter '%.*s'; it's unknown.",
                 len, filter + offset);
      } else {
        ret = filterobj->func(gm, value, value_len, &value_size);
        if (ret != 0) {
          grok_log(gm->grok, LOG_REACTION,
                   "Applying filter '%.*s' returned error %d for string '%.*s'.",
                   len, filter + offset, *value_len, *value);
        }
      }
      offset += len + 1;
      len = 0;
    }

    len++;
  }

  /* We'll always have one filter left over */
  grok_log(gm->grok, LOG_REACTION, "Filter code: %.*s", len, filter + offset);
  filterobj = string_filter_lookup(filter + offset, len);
  if (filterobj == NULL) {
    grok_log(gm->grok, LOG_REACTION, 
             "Can't apply filter '%.*s'; it's unknown.",
             len, filter + offset);
  } else {
    ret = filterobj->func(gm, value, value_len, &value_size);
    if (ret != 0) {
      grok_log(gm->grok, LOG_REACTION,
               "Applying filter '%.*s' returned error %d for string '%.*s'.",
               len, filter + offset, *value_len, *value);
    }
  }

  return *value;

}
