#include "grok.h"
#include "grok_capture.h"
#include "grok_capture_xdr.h"

#define CAPTURE_NUMBER_NOT_SET (-1)

void grok_capture_init(grok_t *grok, grok_capture *gct) {
  gct->id = CAPTURE_NUMBER_NOT_SET;
  gct->pcre_capture_number = CAPTURE_NUMBER_NOT_SET;

  gct->name = NULL;
  gct->pattern = NULL;
  gct->predicate_lib = NULL;
  gct->predicate_func_name = NULL;
}

void grok_capture_add(grok_t *grok, grok_capture *gct) {
  DB *cap_by_id;
  DBT key, value;

  grok_log(grok, LOG_REGEXPAND, "Adding pattern '%s' as capture %d",
             pattern_name, capture_id);

  cap_by_id = grok->captures_by_id;

  /* Primary key is id */
  key.data = &(gct->id);
  key.size = sizeof(gct->id);

  _grok_capture_encode(gct, (char **)&value.data, &value.size);

  cap_by_id->put(cap_by_id, NULL, &key, &value, 0);
}

void grok_capture_get_by_id(grok_t *grok, int id, grok_capture *gct) {
  DB *cap_by_id;
  DBT key, value;
  cap_by_id = grok->captures_by_id;

  key.data = &id;
  key.size = sizeof(id);
  cap_by_id->get(cap_by_id, NULL, &key, &value, 0);

  _grok_capture_decode(gct, (char *)value.data, value.size);
}

void _grok_capture_encode(grok_capture *gct, char **data_ret,
                                    int *size_ret) {
  XDR xdr;
  #define BASE_ALLOC_SIZE 64;
  int alloc_size = BASE_ALLOC_SIZE;
  *data_ret = NULL;

  do {
    if (*data_ret == NULL) {
      *data_ret = malloc(alloc_size);
    } else {
      xdr_destroy(&xdr);
      alloc_size *= 2;
      //fprintf(stderr, "Growing xdr buffer to %d\n", alloc_size);
      *data_ret = realloc(*data_ret, alloc_size);
    }
    xdrmem_create(&xdr, *data_ret, alloc_size, XDR_ENCODE);

    /* If we get larger than a meg, something is probably wrong. */
    if (alloc_size > 1<<20) {
      abort();
    }
  } while (xdr_grok_capture(&xdr, gct) == FALSE);

  *size_ret = xdr_getpos(&xdr);
}

void _grok_capture_decode(grok_capture *gct, char *data, int size) {
  XDR xdr;

  xdrmem_create(&xdr, data, size, XDR_DECODE);
  xdr_grok_capture(&xdr, gct);
}

int _db_captures_by_name_key(DB *secondary, const DBT *key,
                                   const DBT *data, DBT *result) {
  result->data = ((grok_capture *)data->data)->name;
  result->size = strlen((char *)result->data);
  return 0;
}

int _db_captures_by_capture_number(DB *secondary, const DBT *key,
                                         const DBT *data, DBT *result) {
  result->data = & ((grok_capture *)data->data)->pcre_capture_number;
  result->size = sizeof( ((grok_capture*)data->data)->pcre_capture_number );
  return 0;
}
