#ifndef _GROK_CAPTURE_INTERNAL_H_
#define _GROK_CAPTURE_INTERNAL_H_

#include "grok.h"
#include "grok_capture_xdr.h"

void grok_capture_init(grok_t *grok, grok_capture *gct);
void grok_capture_add(grok_t *grok, grok_capture *gct);
void grok_capture_get_by_id(grok_t *grok, int id, grok_capture *gct);


void _grok_capture_encode(grok_capture *gct, char **data_ret, int *size_ret);
void _grok_capture_decode(grok_capture *gct, char *data, int size);

#endif /* _GROK_CAPTURE_INTERNAL_H_ */
