#include <assert.h>
#include <string.h>
#include <errno.h>

#include "grok.h"
#include "grok_pattern.h"

int grok_pattern_add(grok_t *grok, const char *name, size_t name_len,
                      const char *regexp, size_t regexp_len) {
  DB *patterns = grok->patterns;
  DBT key, value;
  memset(&key, 0, sizeof(DBT));
  memset(&value, 0, sizeof(DBT));

  grok_log(grok, LOG_PATTERNS, "Adding new pattern '%.*s' => '%.*s'",
           name_len, name, regexp_len, regexp);

  key.data = (void *)name;
  key.size = name_len;
  value.data = (void *)regexp;
  value.size = regexp_len;

  patterns->put(patterns, NULL, &key, &value, 0);

  return GROK_OK;
}

int grok_pattern_find(grok_t *grok, const char *name, size_t name_len,
                      char **regexp, size_t *regexp_len) {
  DB *patterns = grok->patterns;
  DBT key, value;
  int ret;
  memset(&key, 0, sizeof(DBT));
  memset(&value, 0, sizeof(DBT));

  key.data = (void *)name;
  key.size = name_len;
  ret = patterns->get(patterns, NULL, &key, &value, 0);
  if (ret != 0) {
    grok_log(grok, LOG_PATTERNS, "Searching for pattern '%s': not found", name);
    *regexp = NULL;
    *regexp_len = 0;
    return GROK_ERROR_PATTERN_NOT_FOUND;
  }

  grok_log(grok, LOG_PATTERNS, "Searching for pattern '%s': %.*s",
           name, value.size, value.data);
  *regexp = malloc(value.size);
  memcpy(*regexp, value.data, value.size);
  *regexp_len = value.size;
  return GROK_OK;
}

int grok_patterns_import_from_file(grok_t *grok, const char *filename) {
  FILE *patfile = NULL;
  size_t filesize = 0;
  size_t bytes = 0;
  char *buffer = NULL;

  grok_log(grok, LOG_PATTERNS, "Importing pattern file: '%s'", filename);

  patfile = fopen(filename, "r");
  if (patfile == NULL) {
    grok_log(grok, LOG_PATTERNS, "Unable to open '%s' for reading: %s",
             filename, strerror(errno));
    return GROK_ERROR_FILE_NOT_ACCESSIBLE;
  }
  
  fseek(patfile, 0, SEEK_END);
  filesize = ftell(patfile);
  fseek(patfile, 0, SEEK_SET);
  buffer = calloc(1, filesize + 1);
  if (buffer == NULL) {
    fprintf(stderr, "Fatal: calloc(1, %zd) failed while trying to read '%s'",
            filesize, filename);
    abort();
  }
  memset(buffer, 0, filesize);
  bytes = fread(buffer, 1, filesize, patfile);
  if (bytes != filesize) {
    grok_log(grok, LOG_PATTERNS, "Unable to open '%s' for reading: %s",
             filename, strerror(errno));
    fprintf(stderr, "Expected %zd bytes, but read %zd.", filesize, bytes);
    return GROK_ERROR_UNEXPECTED_READ_SIZE;
  }

  grok_patterns_import_from_string(grok, buffer);

  free(buffer);
  fclose(patfile);
  return GROK_OK;
}

int grok_patterns_import_from_string(grok_t *grok, const char *buffer) {
  char *tokctx = NULL;
  char *tok = NULL;
  char *strptr = NULL;
  char *dupbuf = NULL;

  grok_log(grok, LOG_PATTERNS, "Importing patterns from string");

  dupbuf = strdup(buffer);
  strptr = dupbuf;

  while ((tok = strtok_r(strptr, "\n", &tokctx)) != NULL) {
    const char *name, *regexp;
    size_t name_len, regexp_len;
    strptr = NULL;

    /* skip leading whitespace */
    tok += strspn(tok, " \t");

    /* If first non-whitespace is a '#', then this is a comment. */
    if (*tok == '#') continue;

    _pattern_parse_string(tok, &name, &name_len, &regexp, &regexp_len);
    (void) grok_pattern_add(grok, name, name_len, regexp, regexp_len);
  }

  free(dupbuf);
  return GROK_OK;
}

void _pattern_parse_string(const char *line,
                           const char **name, size_t *name_len,
                           const char **regexp, size_t *regexp_len) {
  size_t offset;

  /* Skip leading whitespace */
  offset = strspn(line, " \t");
  *name = line + offset;

  /* Find the first whitespace */
  offset += strcspn(line + offset, " \t");
  *name_len = offset - (*name - line);

  offset += strspn(line + offset, " \t");
  *regexp = line + offset;
  *regexp_len = strlen(line) - (*regexp - line);
}
