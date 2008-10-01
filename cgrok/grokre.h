#ifndef _GROKRE_H_
#define _GROKRE_H_

void grok_init(grok_t *grok);
void grok_free(grok_t *grok);
void grok_patterns_import_from_file(grok_t *grok, const char *filename);
void grok_patterns_import_from_string(grok_t *grok, char *buffer);

int grok_compile(grok_t *grok, const char *pattern);
int grok_exec(grok_t *grok, const char *text, grok_match_t *gm);

#endif /* _GROKRE_H_ */
