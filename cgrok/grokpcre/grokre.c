#include <pcre.h>
#include <string.h> #include <stdlib.h>
#include <stdio.h>

typedef struct grok_pattern {
  char *name;
  //pcre *pcre;
  char *regexp;
  char *expanded_regexp;
} grok_pattern_t;

typedef struct grok_pattern_set {
  grok_pattern_t *patterns;
  size_t count;
  size_t size;
} grok_pattern_set_t;

typedef struct grok {
  pcre *named_pattern_re;
  grok_pattern_set_t pattern_set;

  /* PCRE pattern compilation errors */
  const char *pcre_errptr;
  int pcre_erroffset;

} grok_t;

/* public functions */
void grok_init(grok_t *grok);

void grok_patterns_import_from_file(grok_t *grok, const char *filename);
void grok_patterns_import_from_string(grok_t *grok, char *buffer);
void grok_pattern_add(grok_t *grok, grok_pattern_t *pattern);

static int grok_pattern_set_cmp_name(const void *a, const void *b);
static void _pattern_parse_string(const char *line, grok_pattern_t *pattern_ret);

static int grok_pattern_set_cmp_name(const void *a, const void *b) {
  grok_pattern_t *ga = (grok_pattern_t *)a;
  grok_pattern_t *gb = (grok_pattern_t *)b;

  return strcmp(ga->name, gb->name);
}

void grok_init(grok_t *grok) {
  grok_pattern_set_t *pset = &grok->pattern_set;
  pset->count = 0;
  pset->size = 20;
  pset->patterns = malloc(pset->size * sizeof(grok_pattern_t));
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

pcre *grok_compile(grok_t *grok, const char *pattern) {
  pcre *re = NULL;
  int *ovector = NULL;
  int num_captures = -1;
  int i = 0;
  int rc = 0;
  int offset, len;

  re = pcre_compile("%((?:[^\\%]+|\\.)+)%", 0, 
                    &grok.pcre_errptr, &grok.pcre_erroffset,
                    NULL);

  if (re == NULL) {
    printf("Regex error: %s\n", grok.pcre_errptr);
    return 1;
  }

  pcre_fullinfo(re, NULL, PCRE_INFO_CAPTURECOUNT, &num_captures);
  num_captures++; /* include the 0th group */

  ovector = malloc( (3 * num_captures) * sizeof(int) );

  offset = 0;
  len = strlen(argv[1]);
  while (rc >= 0) {
    rc = pcre_exec(re, NULL, argv[1] + offset, len - offset, 0, 0,
                   ovector, num_captures * 3);

    if (rc < 0)
      break;

    if (rc == 0)
      rc = num_captures;

    //for (i = 0; i < rc; i++ ) {
    for (i = 1; i < 2; i++ ) {
      const char *sptr;
      pcre_get_substring(argv[1] + offset, ovector, rc, i, &sptr);
      printf("start:%d, end:%d\n", ovector[0], ovector[1]);
      printf("%d: %s\n", i, sptr);
      pcre_free_substring(sptr);

    }
    offset += ovector[1];
  }

  return 0;
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

int main(int argc, const char **argv) {
  grok_t grok;
  grok_pattern_t *gpt = NULL;
  grok_pattern_t key;

  grok_init(&grok);
  grok_patterns_import_from_file(&grok, "../patterns");

  if (argc == 1) {
    printf("Usage: $0 <pattern>\n");
    return 1;
  }

  return 0;
}



#if 0
  key.name = strdup(argv[1]);
  key.regexp = NULL;

  gpt = bsearch(&key, grok.pattern_set.patterns, grok.pattern_set.count, 
                sizeof(grok_pattern_t), grok_pattern_set_cmp_name);

  if (gpt != NULL) {
    printf("Found: %s\n", gpt->name);
    printf("%s\n", gpt->regexp);
  } else {
    printf("Not found: %s\n", key.name);
  }

  return 0;
#endif 
