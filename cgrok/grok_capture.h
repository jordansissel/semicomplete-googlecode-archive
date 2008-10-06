#ifndef _GROK_CAPTURE_INTERNAL_H_
#define _GROK_CAPTURE_INTERNAL_H_

#include "grok.h"

/* grok_capture_xdr defines this already, but if we already included that
 * header, don't define this typedef again 
 *
 * This means if you include grok_capture_xdr.h and grok_capture.h, you have
 * to include _xdr.h first.
 */
#ifndef NEED_GROK_CAPTURE_XDR_H
#include "grok_capture_xdr.h"
#endif

void grok_capture_init(grok_t *grok, grok_capture *gct);
int grok_capture_add(grok_t *grok, grok_capture *gct);
int grok_capture_get_by_id(grok_t *grok, int id, grok_capture *gct);
int grok_capture_get_by_name(grok_t *grok, char *name, grok_capture *gct);
int grok_capture_get_by_capture_number(grok_t *grok, int capture_number,
                                        grok_capture *gct);

void _grok_capture_encode(grok_capture *gct, char **data_ret, int *size_ret);
void _grok_capture_decode(grok_capture *gct, char *data, int size);
int _grok_capture_get_db(grok_t *grok, DB *db, DBT *key, grok_capture *gct);


int _db_captures_by_name_key(DB *secondary, const DBT *key,
                            const DBT *data, DBT *result);

int _db_captures_by_capture_number(DB *secondary, const DBT *key,
                                  const DBT *data, DBT *result);

#endif /* _GROK_CAPTURE_INTERNAL_H_ */
