#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* replace a string range with another string.
 *
 * if replace_len < 0, it is calculated from 'replace'
 * if strp_len < 0, it is calculated from *strp
 * if end < 0, it is set to start
 */
void substr_replace(char **strp, 
                    int *strp_len, int *strp_alloc_size,
                    int start, int end, 
                    const char *replace, int replace_len) {
  int total_len;

  if (replace_len < 0)
    replace_len = strlen(replace);
  if (*strp_len < 0)
    *strp_len = strlen(*strp);
  if (end < 0)
    end = start;

  total_len = *strp_len + replace_len - (end - start);

  if (total_len >= *strp_alloc_size) {
    *strp_alloc_size = total_len + 1;
    *strp = realloc(*strp, *strp_alloc_size);
  }

  memmove(*strp + start + replace_len,
          *strp + end,
          *strp_len - end);
  memcpy(*strp + start, replace, replace_len);
  *strp_len = start + (*strp_len - end)+  replace_len;
}
