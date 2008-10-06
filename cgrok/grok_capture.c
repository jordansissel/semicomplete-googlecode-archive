#include "grok.h"
#include "grok_capture.h"
#include "grok_capture_xdr.h"

#include <assert.h>

#define CAPTURE_NUMBER_NOT_SET (-1)

void grok_capture_init(grok_t *grok, grok_capture *gct) {
  gct->id = CAPTURE_NUMBER_NOT_SET;
  gct->pcre_capture_number = CAPTURE_NUMBER_NOT_SET;

  gct->name = NULL;
  gct->pattern = NULL;
  gct->predicate_lib = NULL;
  gct->predicate_func_name = NULL;
}

int grok_capture_add(grok_t *grok, grok_capture *gct) {
  DB *db = grok->captures_by_id;
  DBT key, value;
  int ret;

  grok_log(grok, LOG_REGEXPAND, "Adding pattern '%s' as capture %d",
             pattern_name, capture_id);

  /* Primary key is id */
  memset(&key, 0, sizeof(key));
  memset(&value, 0, sizeof(value));
  key.data = &(gct->id);
  key.size = sizeof(gct->id);

  _grok_capture_encode(gct, (char **)&value.data, &value.size);
  ret = db->put(db, NULL, &key, &value, 0);
  free(value.data);
  return ret;
}

int _grok_capture_get_db(grok_t *grok, DB *db, DBT *key, grok_capture *gct) {
  DBT value;
  int ret;

  memset(&value, 0, sizeof(DBT));

  ret = db->get(db, NULL, key, &value, 0);
  if (ret != 0) {
    return ret;
  }
  _grok_capture_decode(gct, (char *)value.data, value.size);
  return 0;
}

int grok_capture_get_by_id(grok_t *grok, int id, grok_capture *gct) {
  DBT key;
  int ret;
  memset(&key, 0, sizeof(DBT));
  key.data = &id;
  key.size = sizeof(id);
  ret = _grok_capture_get_db(grok, grok->captures_by_id, &key, gct);
  return ret;
}

int grok_capture_get_by_name(grok_t *grok, char *name,
                              grok_capture *gct) {
  DBT key;
  int ret;
  memset(&key, 0, sizeof(DBT));
  key.data = name;
  key.size = strlen(name);
  ret = _grok_capture_get_db(grok, grok->captures_by_name, &key, gct);
  return ret;
}

int grok_capture_get_by_capture_number(grok_t *grok, int capture_number,
                                        grok_capture *gct) {
  DBT key;
  int ret;
  memset(&key, 0, sizeof(DBT));
  key.data = &capture_number;
  key.size = sizeof(capture_number);
  ret = _grok_capture_get_db(grok, grok->captures_by_capture_number,
                             &key, gct);
  return ret;
}

void _grok_capture_encode(grok_capture *gct, char **data_ret,
                                    int *size_ret) {
  #define BASE_ALLOC_SIZE 64;
  XDR xdr;
  grok_capture local;
  int alloc_size = BASE_ALLOC_SIZE;
  *data_ret = NULL;

  /* xdr_string doesn't understand NULL, so replace NULL with "" */
  memcpy(&local, gct, sizeof(local));
  if (local.name == NULL) 
    local.name = "";
  if (local.pattern == NULL)
    local.pattern = "";
  if (local.predicate_lib == NULL)
    local.predicate_lib = "";
  if (local.predicate_func_name == NULL)
    local.predicate_func_name = "";

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
  } while (xdr_grok_capture(&xdr, &local) == FALSE);

  *size_ret = xdr_getpos(&xdr);
}

void _grok_capture_decode(grok_capture *gct, char *data, int size) {
  XDR xdr;

  xdrmem_create(&xdr, data, size, XDR_DECODE);
  xdr_grok_capture(&xdr, gct);
  //xdr_grok_capture(&xdr, &foo);
}

int _db_captures_by_name_key(DB *secondary, const DBT *key,
                                   const DBT *data, DBT *result) {
  grok_capture gct;
  int len;

  grok_capture_init(NULL, &gct);
  _grok_capture_decode(&gct, (char *)data->data, data->size);

  if (gct.name == NULL)
    return DB_DONOTINDEX;

  len = strlen(gct.name);

  result->data = malloc(len);
  memcpy(result->data, gct.name, len);
  result->size = len;

  //printf("Added 2key name: '%.*s'\n", len, gct.name);
  result->flags |= DB_DBT_APPMALLOC;
  return 0;
}

int _db_captures_by_capture_number(DB *secondary, const DBT *key,
                                         const DBT *data, DBT *result) {
  grok_capture gct;
  grok_capture_init(NULL, &gct);
  _grok_capture_decode(&gct, (char *)data->data, data->size);

  if (gct.pcre_capture_number == CAPTURE_NUMBER_NOT_SET)
    return DB_DONOTINDEX;

  result->data = malloc(sizeof(int));
  *((int *)result->data) = gct.pcre_capture_number;
  result->size = sizeof(int);

  result->flags |= DB_DBT_APPMALLOC;
  return 0;
}
