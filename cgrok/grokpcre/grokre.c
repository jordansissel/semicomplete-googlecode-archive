#include <pcre.h>
#include <string.h> 
#include <stdlib.h>
#include <stdio.h>

typedef struct grok_pattern {
  char *name;
  char *regexp;
} grok_pattern_t;

typedef struct grok_pattern_set {
  grok_pattern_t *patterns;
  size_t count;
  size_t size;
} grok_pattern_set_t;

typedef struct grok {
  grok_pattern_set_t pattern_set;
  
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

/* global, static variables */

/* pattern to match %{FOO:BAR} */
#define PATTERN_REGEX "%{" \
                        "(?<name>" \
                          "(?<pattern>[A-z0-9._-]+)" \
                          "(?::(?<subname>[A-z0-9._-]+))?" \
                        ")" \
                      "}"

static pcre *g_pattern_re = NULL;

/* public functions */
void grok_init(grok_t *grok);

void grok_patterns_import_from_file(grok_t *grok, const char *filename);
void grok_patterns_import_from_string(grok_t *grok, char *buffer);
void grok_pattern_add(grok_t *grok, grok_pattern_t *pattern);
int grok_compile(grok_t *grok, const char *pattern);
int grok_exec(grok_t *grok, const char *text);
char *grok_pattern_expand(grok_t *grok);

static int grok_pattern_set_cmp_name(const void *a, const void *b);
static void _pattern_parse_string(const char *line, grok_pattern_t *pattern_ret);

static int grok_pattern_set_cmp_name(const void *a, const void *b) {
  grok_pattern_t *ga = (grok_pattern_t *)a;
  grok_pattern_t *gb = (grok_pattern_t *)b;
  return strcmp(ga->name, gb->name);
}

void grok_init(grok_t *grok) {
  grok_pattern_set_t *pset = &grok->pattern_set;

  memset(grok, 0, sizeof(grok_t));
  pset->count = 0;
  pset->size = 20;
  pset->patterns = malloc(pset->size * sizeof(grok_pattern_t));

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
  }
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
  grok_pattern_set_t *pset = &grok->pattern_set;

  dupbuf = strdup(buffer);
  strptr = dupbuf;

  while ((tok = strtok_r(strptr, "\n", &tokctx)) != NULL) {
    grok_pattern_t tmp;
    strptr = NULL;

    /* skip leading whitespace */
    tok += strspn(tok, " \t");

    /* If first non-whitespace is a '#', then this is a comment. */
    if (*tok == '#') continue;

    _pattern_parse_string(tok, &tmp);
    grok_pattern_add(grok, &tmp);

  }

  qsort(pset->patterns, pset->count, sizeof(grok_pattern_t), 
        grok_pattern_set_cmp_name);

  free(dupbuf);
}

void grok_pattern_add(grok_t *grok, grok_pattern_t *pattern) {
  grok_pattern_set_t *pset = &grok->pattern_set;

  pset->patterns[pset->count].name = pattern->name;
  pset->patterns[pset->count].regexp = pattern->regexp;
  pset->count++;
  if (pset->count == pset->size) {
    pset->size *= 2;
    pset->patterns = realloc(pset->patterns, pset->size * sizeof(grok_pattern_t));
    /* Check for null here */
  }
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

int grok_exec(grok_t *grok, const char *text) {
  int ret;
  ret = pcre_exec(grok->re, NULL, text, strlen(text), 0, 0,
                  grok->capture_vector, grok->num_captures);
  switch (ret) {
    case PCRE_ERROR_NOMATCH:
      printf("'%s' vs '%s'\n", grok->full_pattern, text);
      printf("No match\n");
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
    default:
      printf("MATCH: %d\n", ret);
  }

  return ret;
}

char *grok_pattern_expand(grok_t *grok) {
  return strdup(grok->pattern);
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
  grok_pattern_t *gpt = NULL;
  grok_pattern_t key;

  grok_init(&grok);
  grok_patterns_import_from_file(&grok, "../patterns");

  if (argc != 3) {
    printf("Usage: $0 <pattern> <text>\n");
    return 1;
  }

  grok_compile(&grok, argv[1]);
  grok_exec(&grok, argv[2]);

  return 0;
}
