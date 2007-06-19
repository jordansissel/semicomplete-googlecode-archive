/*
 * xdo library
 *
 */

#include <X11/Xlib.h>
#include "xdo_util.h"

typedef struct xdo {
  Display *xdpy;
  char *display_name;
  charcodemap_t *charcodes;
  int keycode_high; /* highest and lowest keycodes */
  int keycode_low;  /* used by this X server */
} xdo_t;

xdo_t* xdo_new(char *display);
void xdo_free(xdo_t *xdo);

void xdo_mousemove(xdo_t *xdo, int x, int y);
//void xdo_motion_relative(xdo_t *xdo, int x, int y);
void xdo_mousedown(xdo_t *xdo, int button);
void xdo_mouseup(xdo_t *xdo, int button);

void xdo_click(xdo_t *xdo, int button);

void xdo_type(xdo_t *xdo, char *string);
void xdo_keysequence(xdo_t *xdo, char *keysequence);
