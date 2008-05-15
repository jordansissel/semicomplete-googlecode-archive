#include <pcre.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct grok_pattern {
  char *name;
  //pcre *pcre;
  char *regexp;
  char *expanded_regexp;
} grok_pattern_t;

typedef struct grok_pattern_set {
  grok_pattern_t *patterns;
  size_t num_patterns;
  size_t max_patterns;
} grok_pattern_set_t;

typedef struct grok_context {
  pcre *named_pattern_re;
  grok_pattern_set_t *pattern_set;
} grok_context_t;

/* XXX: Remove this in favor of grok_context_t */
static pcre *global_named_re;

int grok_pattern_name_cmp(const void *a, const void *b);
void read_patterns(const char *filename, grok_pattern_set_t *pattern_set);
void parse_patterns(char *buffer, grok_pattern_set_t *pattern_set);
void parse_pattern_line(const char *line, grok_pattern_t *pattern);
void expand_patterns(grok_pattern_set_t *pattern_set);

int grok_pattern_name_cmp(const void *a, const void *b) {
  grok_pattern_t *ga = (grok_pattern_t *)a;
  grok_pattern_t *gb = (grok_pattern_t *)b;

  return strcmp(ga->name, gb->name);
}

void read_patterns(const char *filename, grok_pattern_set_t *pattern_set) {
  FILE *patfile = NULL;
  size_t filesize;
  size_t bytes;
  char *buffer;
  int ret;

  patfile = fopen(filename, "r");
  if (patfile == NULL) {
    fprintf(stderr, "Unable to open '%s' for reading\n", filename);
    perror("Error: ");
    return;
  }

  fseek(patfile, 0, SEEK_END);
  filesize = ftell(patfile);
  fseek(patfile, 0, SEEK_SET);
  buffer = malloc(filesize);
  memset(buffer, 0, filesize);
  bytes = fread(buffer, 1, filesize, patfile);
  if (bytes != filesize) {
    fprintf(stderr, "Expected %zd bytes, but read %zd.", filesize, bytes);
    return;
  }

  parse_patterns(buffer, pattern_set);

  free(buffer);
  fclose(patfile);
}

void parse_patterns(char *buffer, grok_pattern_set_t *pattern_set) {
  char *tokctx = NULL;
  char *tok = NULL;
  char *strptr = NULL;
  char *dupbuf = NULL;

  dupbuf = strdup(buffer);
  strptr = dupbuf;

  if (pattern_set->patterns != NULL) {
    free(pattern_set->patterns);
    pattern_set->patterns = NULL;
  }

  pattern_set->max_patterns = 20;
  pattern_set->patterns= malloc(pattern_set->max_patterns * sizeof(grok_pattern_t));
  pattern_set->num_patterns = 0;

  while ((tok = strtok_r(strptr, "\n", &tokctx)) != NULL) {
    grok_pattern_t tmp;
    strptr = NULL;

    // skip leading whitespace
    tok += strspn(tok, " \t");

    // ignore comments;
    if (*tok == '#') continue;

    parse_pattern_line(tok, &tmp);
    //printf("Found pattern: %s\n", tmp.name);
    //printf(" -> %s\n", tmp.regexp);
    pattern_set->patterns[pattern_set->num_patterns].name = tmp.name;
    pattern_set->patterns[pattern_set->num_patterns].regexp = tmp.regexp;

    pattern_set->num_patterns++;
    if (pattern_set->num_patterns == pattern_set->max_patterns) {
      pattern_set->max_patterns *= 2;
      pattern_set->patterns = realloc(pattern_set->patterns, 
                                      pattern_set->max_patterns * sizeof(grok_pattern_t)); 
    }
  }

  qsort(pattern_set->patterns, pattern_set->num_patterns, sizeof(grok_pattern_t), 
        grok_pattern_name_cmp);

  expand_patterns(pattern_set);
  
  free(dupbuf);
}

void parse_pattern_line(const char *line, grok_pattern_t *pattern) {
  char *linedup = strdup(line);
  char *name = linedup;
  char *regexp = NULL;
  size_t offset;

  /* Find the first whitespace */
  offset = strcspn(line, " \t");
  linedup[offset] = '\0';
  offset += strspn(linedup + offset, " \t");
  regexp = linedup + offset + 1;

  pattern->name = strdup(name);
  pattern->regexp = strdup(regexp);

  free(linedup);
}

void expand_patterns(grok_pattern_set_t *pattern_set) {
}

int main(int argc, const char **argv) {
  grok_pattern_set_t pset;
  pset.patterns = NULL;
  grok_pattern_t *gpt = NULL;
  grok_pattern_t key;

  //global_named_re = pcre_compile("%(?:[^\\%]+|\\.)+%");
  read_patterns("../patterns", &pset);

  key.name = strdup(argv[1]);
  key.regexp = NULL;

  gpt = bsearch(&key, pset.patterns, pset.num_patterns, sizeof(grok_pattern_t),
                grok_pattern_name_cmp);

  if (gpt != NULL) {
    printf("Found: %s\n", gpt->name);
    printf("%s\n", gpt->regexp);
  } else {
    printf("Not found: %s\n", key.name);
  }

  return 0;
}

int old_main(int argc, char **argv) {
  pcre *re;
  const char *error;
  int erroffset = -1;
  int rc = -1;
  int i = -1;
  int numcaptures = -1;
  int *ovector = NULL;

  char *input = argv[1];
  re = pcre_compile("(?(DEFINE)(?<test>abcdef))(foo)([^ ]+) (bar.*)",
                    0, &error, &erroffset, NULL);

  if (re == NULL) {
    printf("!Regex error: %s\n", error);
    return 1;
  }

  pcre_fullinfo(re, NULL, PCRE_INFO_CAPTURECOUNT, &numcaptures);
  numcaptures+=1; /* include the 0th group */

  ovector = malloc((3 * numcaptures) * sizeof(int));
  rc = pcre_exec(re, NULL, input, (int)strlen(input), 0, 0, 
                 ovector, numcaptures * 3);

  if (rc < 0) {
    if (rc == PCRE_ERROR_NOMATCH) {
      printf("No match\n");
      return 2;
    } else {
      printf("Match error: %d\n", rc);
      return 3;
    }
  }

  printf("rc: %d\n", rc);
  printf("numcaptures: %d\n", numcaptures);

  if (rc == 0)
    rc = numcaptures;

  for (i = 0; i < rc; i++) {
    const char *sptr;
    pcre_get_substring(input, ovector, rc, i, &sptr);
    printf("%d: %s\n", i, sptr);
    pcre_free_substring(sptr);
  }
}
