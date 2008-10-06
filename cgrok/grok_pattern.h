#ifndef _GROK_PATTERN_H_
#define _GROK_PATTERN_H_

void grok_pattern_add(grok_t *grok, const char *name, size_t name_len, const char *regexp, size_t regexp_len);
const char *grok_pattern_find(grok_t *grok, const char *name, size_t name_len);
void grok_patterns_import_from_file(grok_t *grok, const char *filename);
void grok_patterns_import_from_string(grok_t *grok, const char *buffer);

/* Exposed only for testing */
void _pattern_parse_string(const char *line,
                           const char **name, size_t *name_len,
                           const char **regexp, size_t *regexp_len);

#endif /* _GROK_PATTERN_H_ */
