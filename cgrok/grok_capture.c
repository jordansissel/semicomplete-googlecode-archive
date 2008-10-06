#include "grok.h"
#include "grok_capture.h"

void grok_capture_add(grok_t *grok, int capture_id,
                      char *pattern_name)  {
  DB *cap_by_id;
  grok_capture gct;

  grok_log(grok, LOG_REGEXPAND, "Adding pattern '%s' as capture %d",
             pattern_name, capture_id);
  
  gct.id = capture_id;
  gct.name = pattern_name;
  gct.predicate_func_name = NULL;
  gct.pattern = NULL;
}

static void _grok_capture_encode(grok_capture *gct, char **data_ret,
                                    int *size_ret) {
  XDR xdr;
  int alloc_size = 64;
  int ret = FALSE;
  *data_ret == NULL;

  do {
    alloc_size *= 2;
    if (*data_ret == NULL) {
      *data_ret = malloc(alloc_size);
    } else {
      *data_ret = realloc(*data_ret, alloc_size);
    }
    xdrmem_create(&xdr, *data_ret, alloc_size, XDR_ENCODE);
  } while (xdr_grok_capture(&xdr, gct) == FALSE);
}

void _grok_capture_decode(grok_capture *gct, char *data, int size) {
  XDR xdr;

  xdrmem_create(&xdr, data, size, XDR_DECODE);
  xdr_grok_capture(&xdr, gct);
}
