#include <pcre.h>
#include <assert.h>
#include <string.h> 
#include <stdlib.h>
#include <stdio.h>
#include <search.h>

typedef struct grok_pattern {
  const char *name;
  char *regexp;
} grok_pattern_t;

typedef struct grok {
  /* tree of grok_pattern objects */
  void *patterns;
  
  /* These are initialized when grok_compile is called */
  pcre *re;
  const char *pattern;
  char *full_pattern;
  int *capture_vector;
  int num_captures;

  /* PCRE pattern compilation errors */
  const char *pcre_errptr;
  int pcre_erroffset;
} grok_t;

typedef struct grok_match {
  const grok_t *gre;
  const char *subject;
} grok_match_t;

/* global, static variables */

/* pattern to match %{FOO:BAR} */
#define PATTERN_REGEX "%{" \
                        "(?<name>" \
                          "(?<pattern>[A-z0-9._-]+)" \
                          "(?::(?<subname>[A-z0-9._-]+))?" \
                        ")" \
                      "}"

static pcre *g_pattern_re = NULL;
static int g_pattern_num_captures = 0;

void pwalk(const void *node, VISIT visit, int nodelevel);

/* public functions */
void grok_init(grok_t *grok);
void grok_free(grok_t *grok);

void grok_patterns_import_from_file(grok_t *grok, const char *filename);
void grok_patterns_import_from_string(grok_t *grok, char *buffer);
void grok_pattern_add(grok_t *grok, grok_pattern_t *pattern);
int grok_compile(grok_t *grok, const char *pattern);
int grok_exec(grok_t *grok, const char *text, grok_match_t *gm);
char *grok_pattern_expand(grok_t *grok);
grok_pattern_t *grok_pattern_find(grok_t *grok, const char *pattern_name);

static int grok_pattern_cmp_name(const void *a, const void *b);
static void _pattern_parse_string(const char *line, grok_pattern_t *pattern_ret);

static int grok_pattern_cmp_name(const void *a, const void *b) {
  grok_pattern_t *ga = (grok_pattern_t *)a;
  grok_pattern_t *gb = (grok_pattern_t *)b;
  return strcmp(ga->name, gb->name);
}

void grok_init(grok_t *grok) {
  memset(grok, 0, sizeof(grok_t));

  if (g_pattern_re == NULL) {
    g_pattern_re = pcre_compile(PATTERN_REGEX, 0, 
                                &grok->pcre_errptr,
                                &grok->pcre_erroffset,
                                NULL);
    if (g_pattern_re == NULL) {
      fprintf(stderr, "Internal compiler error: %s\n", grok->pcre_errptr);
      fprintf(stderr, "Regexp: %s\n", PATTERN_REGEX);
      fprintf(stderr, "Position: %d\n", grok->pcre_erroffset);
    }

    pcre_fullinfo(g_pattern_re, NULL, PCRE_INFO_CAPTURECOUNT,
                  &g_pattern_num_captures);
    g_pattern_num_captures++; /* include the 0th group */
  }
}

void grok_free(grok_t *grok) {
  /* twalk grok->patterns and free them? */

  if (grok->re != NULL)
    pcre_free(grok->re);

  if (grok->full_pattern != NULL)
    free(grok->full_pattern);

  if (grok->capture_vector != NULL)
    free(grok->capture_vector);
}

void grok_patterns_import_from_file(grok_t *grok, const char *filename) {
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

  grok_patterns_import_from_string(grok, buffer);

  free(buffer);
  fclose(patfile);
}

void grok_patterns_import_from_string(grok_t *grok, char *buffer) {
  char *tokctx = NULL;
  char *tok = NULL;
  char *strptr = NULL;
  char *dupbuf = NULL;

  dupbuf = strdup(buffer);
  strptr = dupbuf;

  while ((tok = strtok_r(strptr, "\n", &tokctx)) != NULL) {
    grok_pattern_t tmp;
    grok_pattern_t **result;
    strptr = NULL;

    /* skip leading whitespace */
    tok += strspn(tok, " \t");

    /* If first non-whitespace is a '#', then this is a comment. */
    if (*tok == '#') continue;

    _pattern_parse_string(tok, &tmp);
    grok_pattern_add(grok, &tmp);
    result = tfind(&tmp, &(grok->patterns), grok_pattern_cmp_name);
    assert(result != NULL);
    //printf("%s vs %s\n", (*result)->name, tmp.name);
    assert(!strcmp((*result)->name, tmp.name));
  }


  free(dupbuf);
}

void grok_pattern_add(grok_t *grok, grok_pattern_t *pattern) {
  /* tsearch(3) claims it adds a node if it does not exist */
  grok_pattern_t *newpattern;
  void *val;
  const void *ret;

  newpattern = calloc(1, sizeof(grok_pattern_t));
  newpattern->name = strdup(pattern->name);
  newpattern->regexp = strdup(pattern->regexp);

  ret = tsearch(newpattern, &(grok->patterns), grok_pattern_cmp_name);
  if (val == NULL)
    fprintf(stderr, "Failed adding pattern '%s'\n", newpattern->name);
}

int grok_compile(grok_t *grok, const char *pattern) {
  int i = 0;
  int rc = 0;
  int offset, len;

  grok->pattern = pattern;
  grok->full_pattern = grok_pattern_expand(grok);

  grok->re = pcre_compile(grok->full_pattern, 0, 
                          &grok->pcre_errptr, &grok->pcre_erroffset,
                          NULL);

  if (grok->re == NULL) {
    printf("Regex error: %s\n", grok->pcre_errptr);
    return 1;
  }

  pcre_fullinfo(grok->re, NULL, PCRE_INFO_CAPTURECOUNT, &grok->num_captures);
  grok->num_captures++; /* include the 0th group */
  grok->capture_vector = malloc((3 * grok->num_captures) * sizeof(int) );

  return 0;
}

int grok_exec(grok_t *grok, const char *text, grok_match_t *gm) {
  int ret;
  ret = pcre_exec(grok->re, NULL, text, strlen(text), 0, 0,
                  grok->capture_vector, grok->num_captures * 3);
  if (ret < 0) {
    switch (ret) {
      case PCRE_ERROR_NOMATCH:
        break;
      case PCRE_ERROR_NULL:
        printf("Null error, one of the arguments was null?\n");
        break;
      case PCRE_ERROR_BADOPTION:
        printf("badoption\n");
        break;
      case PCRE_ERROR_BADMAGIC:
        printf("badmagic\n");
        break;
    }
    return ret;
  }

  if (gm != NULL) {
    gm->gre = grok;
    gm->subject = text;
  }

  return ret;
}

/* XXX: This method is pretty long; split it up? */
char *grok_pattern_expand(grok_t *grok) {
  int offset = 0;
  int length = strlen(grok->pattern);
  int *capture_vector = malloc(3 * g_pattern_num_captures * sizeof(int));
  int cap_name = pcre_get_stringnumber(g_pattern_re, "name");
  int cap_pattern = pcre_get_stringnumber(g_pattern_re, "pattern");
  int cap_subname = pcre_get_stringnumber(g_pattern_re, "subname");

  int full_len = strlen(grok->pattern);
  int full_size = full_len + 1;
  char *full_pattern = calloc(1, full_size);
  strncpy(full_pattern, grok->pattern, full_len);
  full_pattern[full_len] = '\0';

  printf("F: %08x\n", *(int *)full_pattern);

  const char *patname = NULL;

  while (pcre_exec(g_pattern_re, NULL, full_pattern, full_len, offset, 
                   0, capture_vector, g_pattern_num_captures * 3) >= 0) {
    int start, end, matchlen;
    grok_pattern_t *gpt;

    start = capture_vector[0];
    end = capture_vector[1];
    matchlen = end - start;

    pcre_get_substring(full_pattern, capture_vector, g_pattern_num_captures,
                       cap_pattern, &patname);
    gpt = grok_pattern_find(grok, patname);
    if (gpt == NULL) {
      offset = end;
    } else {
      int patname_len = strlen(patname);
      int regexp_len = strlen(gpt->regexp);
      int repl_len = regexp_len + patname_len + 5;
      int remainder_offset = start + repl_len;
      /* Adding 5 is for the length of "(?<>)" */

      /* The pattern was found, inject it into the full_pattern */
      /* This next section exists to mutate this:
       *   "foo %{TEST} bar"
       * into this:
       *   "foo SOMETHING bar"
       */

      if (full_len + repl_len + 1>= full_size) {
        full_size = (full_len + repl_len) * 2;
        full_pattern = realloc(full_pattern, full_size);
        printf("Fu: %08x\n", *(int *)full_pattern);
        full_pattern[full_size - 1] = '\0';
      }

      /* Invariant, full_pattern actual len must always be full_len */
      assert(strlen(full_pattern) == full_len);

      /* Move the remainder of the string to the end of the 
       * regexp we are injecting */
      /* inject the regexp */
      printf("\n");
      printf("Start: %d, slen: %d, rlen: %d\n", start, full_len, repl_len);
      printf("String:    '%.*s'\n", full_len, full_pattern);
      printf("Replacing: '% *.*s'\n", end, matchlen, full_pattern + start);
      printf("Moving st: '% *s'\n", end, "^");
      printf("Moving to: '% *s'\n", remainder_offset, "^");
      printf("With:      '% *s(?<%s>%s)\n", start, "", patname, gpt->regexp);

      printf("oldstring: %s\n", full_pattern);
      memmove(full_pattern + remainder_offset, full_pattern + end, 
              full_len - matchlen);
      memset(full_pattern + start, '?', remainder_offset - start);
      printf("postmove:  %s\n", full_pattern);
      printf("replacing: % *s\n", start, "^");

      char tmp = full_pattern[start + repl_len];
      printf("Char at   :% *c\n", start + repl_len, tmp);
      snprintf(full_pattern + start, repl_len + 1,
               "(?<%s>%s)", patname, gpt->regexp);
      full_pattern[start + repl_len] = tmp;
      full_pattern[full_len] = '\0';
      printf("snprintf:  %.*s\n", full_len, full_pattern);
      full_len += repl_len - matchlen;
      //printf("newlen2: %d _ %d = %d vs %d\n", repl_len, matchlen, full_len, 
             //strlen(full_pattern));

      /* Invariant, full_pattern actual len must always be full_len */
      assert(strlen(full_pattern) == full_len);
      
      /* Move offset to the start of the regexp pattern we just injected.
       * This is so when we iterate again, we can process this new pattern
       * to see if the regexp included itself any %{FOO} things */
      offset = start;
    }

    if (patname != NULL) {
      pcre_free_substring(patname);
      patname = NULL;
    }
  }

  return full_pattern;
}

grok_pattern_t *grok_pattern_find(grok_t *grok, const char *pattern_name) {
  grok_pattern_t key;
  grok_pattern_t **result;

  key.name = pattern_name;
  key.regexp = NULL;

  result = (grok_pattern_t **) tfind(&key, &(grok->patterns), grok_pattern_cmp_name);
  if (result == NULL)
    return NULL;
  return *result;
}

void _pattern_parse_string(const char *line, grok_pattern_t *pattern_ret) {
  char *linedup = strdup(line);
  char *name = linedup;
  char *regexp = NULL;
  size_t offset;

  /* Find the first whitespace */
  offset = strcspn(line, " \t");
  linedup[offset] = '\0';
  offset += strspn(linedup + offset, " \t");
  regexp = linedup + offset + 1;

  pattern_ret->name = strdup(name);
  pattern_ret->regexp = strdup(regexp);

  free(linedup);
}

int main(int argc, const char * const *argv) {
  grok_t grok;

  grok_init(&grok);
  grok_patterns_import_from_file(&grok, "pcregrok_patterns");

  if (argc != 2) {
    printf("Usage: $0 <pattern>\n");
    return 1;
  }

  grok_compile(&grok, argv[1]);

  if (1) { /* read from stdin, apply the given pattern to it */
    int ret;
    grok_match_t gm;
    char buffer[4096];
    FILE *fp;
    fp = stdin;
    while (!feof(fp)) {
      fgets(buffer, 4096, fp);
      ret = grok_exec(&grok, buffer, &gm);
      if (ret >= 0) {
        int num;
        printf("%s", buffer);
        const char *ip = NULL;
        num = pcre_get_stringnumber(grok.re, "IP");
        pcre_get_substring(buffer, grok.capture_vector,
                           grok.num_captures, num, &ip);
        printf("ip: %s\n", ip);
        pcre_free_substring(ip);
      }
    }
  }
  return 0;
}

void pwalk(const void *node, VISIT visit, int nodelevel) {
  grok_pattern_t *pat = *(grok_pattern_t **)node;
  switch (visit) {
    case preorder:
      break;
    case postorder:
      printf("postorder: %d: %s\n", nodelevel, pat->name);
      break;
    case endorder:
      break;
    case leaf:
      printf("leaf: %d: %s\n", nodelevel, pat->name);
      break;
  }
}
