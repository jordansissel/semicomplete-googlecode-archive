/* xdo library
 */

#include <sys/select.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>

#include "xdo.h"

static void _xdo_keydown(xdo_t *xdo, int keycode);
static void _xdo_keyup(xdo_t *xdo, int keycode);
static void _xdo_keyaction(xdo_t *xdo, int keycode, int state);

static void _xdo_populate_charcode_map(xdo_t *xdo);
static int _xdo_has_xtest(xdo_t *xdo);

static int _xdo_keycode_from_char(xdo_t *xdo, char key);
static int _xdo_get_shiftcode_if_needed(xdo_t *xdo, char key);

/* context-free functions */
char _keysym_to_char(char *keysym);

xdo_t* xdo_new(char *display) {
  xdo_t *xdo = NULL;

  if (display == NULL)
    fprintf(stderr, "xdo_new: display I was given is a null pointer\n");

  /* XXX: Check for NULL here */
  xdo = malloc(sizeof(xdo_t));
  memset(xdo, 0, sizeof(xdo_t));

  /* XXX: Check for NULL here */
  //xdo->display_name = (char *)strdup(display);
  xdo->display_name = display;

  if ((xdo->xdpy = XOpenDisplay(xdo->display_name)) == NULL) {
    fprintf(stderr, "Error: Can't open display: %s\n", xdo->display_name);
    xdo_free(xdo);
    return NULL;
  }

  if (!_xdo_has_xtest(xdo)) {
    fprintf(stderr, "Error: XTEST extension unavailable on '%s'.", 
            xdo->display_name);
    xdo_free(xdo);
    return NULL;
  }

  /* populate the character map? */
  _xdo_populate_charcode_map(xdo);

  return xdo;
}

void xdo_free(xdo_t *xdo) {
  free(xdo->display_name);
  if (xdo->xdpy)
    XCloseDisplay(xdo->xdpy);
  free(xdo);
}

/* XXX: Include 'screen number' support? */
void xdo_mousemove(xdo_t *xdo, int x, int y)  {
  int result;
  result = XTestFakeMotionEvent(xdo->xdpy, -1, x, y, CurrentTime);
  XFlush(xdo->xdpy);
}

void xdo_mousedown(xdo_t *xdo, int button) {
  int result;
  result = XTestFakeButtonEvent(xdo->xdpy, button, True, CurrentTime);
  XFlush(xdo->xdpy);
}

void xdo_mouseup(xdo_t *xdo, int button) {
  int result;
  result = XTestFakeButtonEvent(xdo->xdpy, button, False, CurrentTime);
  XFlush(xdo->xdpy);
}

void xdo_click(xdo_t *xdo, int button) {
  int result;
  xdo_mousedown(xdo, button);
  xdo_mouseup(xdo, button);

  /* no need to flush here */
}

void xdo_type(xdo_t *xdo, char *string) {
  int i = 0;
  char key = '0';
  int keycode = 0;
  int shiftcode = 0;

  for (i = 0; string[i] != '\0'; i++) {
    key = string[i];
    keycode = _xdo_keycode_from_char(xdo, key);
    shiftcode = _xdo_get_shiftcode_if_needed(xdo, key);

    if (shiftcode)
      XTestFakeKeyEvent(xdo->xdpy, shiftcode, True, CurrentTime);
    XTestFakeKeyEvent(xdo->xdpy, keycode, True, CurrentTime);
    XTestFakeKeyEvent(xdo->xdpy, keycode, False, CurrentTime);
    if (shiftcode)
      XTestFakeKeyEvent(xdo->xdpy, shiftcode, False, CurrentTime);

    /* XXX: Flush here or at the end? */
    XFlush(xdo->xdpy);
  }
}

void xdo_keysequence(xdo_t *xdo, char *keyseq) {
  char *tokctx = NULL;
  char *tok = NULL;
  char *strptr = NULL;
  int i;
  
  /* Array of keys to press, in order */
  int *keys = NULL;
  int nkeys = 0;
  int keys_size = 10;

  if (strcspn(keyseq, " \t\n.-[]{}\\|") != strlen(keyseq)) {
    fprintf(stderr, "Error: Invalid key sequence '%s'\n", keyseq);
    return;
  }

  keys = malloc(keys_size * sizeof(int));
  strptr = keyseq;
  while ((tok = strtok_r(strptr, "+", &tokctx)) != NULL) {
    int keysym;
    if (strptr != NULL)
      strptr = NULL;

    /* Check if 'tok' (string keysym) is an alias to another key */
    /* symbol_map comes from xdo.util */
    for (i = 0; symbol_map[i] != NULL; i+=2)
      if (!strcasecmp(tok, symbol_map[i]))
        tok = symbol_map[i + 1];

    keysym = XStringToKeysym(tok);
    if (keysym == NoSymbol) {
      fprintf(stderr, "(symbol) No such key name '%s'. Ignoring it.\n", tok);
      continue;
    }

    keys[nkeys] = XKeysymToKeycode(xdo->xdpy, keysym);

    if (keys[nkeys] == 0) {
      fprintf(stderr, "No such key '%s'. Ignoring it.\n", tok);
      continue;
    }

    nkeys++;
    if (nkeys == keys_size) {
      keys_size *= 2;
      keys = realloc(keys, keys_size);
    }
  }

  for (i = 0; i < nkeys; i++)
    XTestFakeKeyEvent(xdo->xdpy, keys[i], True, CurrentTime);
  for (i = nkeys - 1; i >= 0; i--)
    XTestFakeKeyEvent(xdo->xdpy, keys[i], False, CurrentTime);

  XFlush(xdo->xdpy);
}

/* Helper functions */
static int _xdo_keycode_from_char(xdo_t *xdo, char key) {
  int i = 0;
  int len = xdo->keycode_high - xdo->keycode_low;

  for (i = 0; i < len; i++)
    if (xdo->charcodes[i].key == key)
      return xdo->charcodes[i].code;

  return -1;
}

static int _xdo_get_shiftcode_if_needed(xdo_t *xdo, char key) {
  int i = 0;
  int len = xdo->keycode_high - xdo->keycode_low;

  for (i = 0; i < len; i++)
    if (xdo->charcodes[i].key == key)
      return xdo->charcodes[i].shift;

  return -1;
}

static int _xdo_has_xtest(xdo_t *xdo) {
  int dummy;
  return (XTestQueryExtension(xdo->xdpy, &dummy, &dummy, &dummy, &dummy) == True);
}

static void _xdo_populate_charcode_map(xdo_t *xdo) {
  /* assert xdo->display is valid */
  int keycodes_length = 0;
  int shift_keycode = 0;
  int i, j;

  XDisplayKeycodes(xdo->xdpy, &(xdo->keycode_low), &(xdo->keycode_high));

  /* Double size of keycode range because some 
   * keys have "shift" values. ie; 'a' and 'A', '2' and '@' */
  keycodes_length = (xdo->keycode_high - xdo->keycode_low) * 2 + 1;
  xdo->charcodes = malloc(keycodes_length * sizeof(charcodemap_t));
  memset(xdo->charcodes, 0, keycodes_length * sizeof(charcodemap_t));

  /* Fetch the keycode for Shift_L */
  /* XXX: Make 'Shift_L' configurable? */
  shift_keycode = XKeysymToKeycode(xdo->xdpy, XStringToKeysym("Shift_L"));

  for (i = xdo->keycode_low; i <= xdo->keycode_high; i++) {
    char *keybuf = 0;

    /* Index '0' in KeycodeToKeysym == no shift key
     * Index '1' in ... == shift key held
     * hence this little loop. */
    for (j = 0; j <= 1; j++) { 
     int index = (i - xdo->keycode_low) * 2 + j;
     keybuf = XKeysymToString(XKeycodeToKeysym(xdo->xdpy, i, j));

     xdo->charcodes[index].key = _keysym_to_char(keybuf);
     xdo->charcodes[index].code = i;
     xdo->charcodes[index].shift = j ? shift_keycode : 0;
     printf("index; %d %d\n", index, xdo->keycode_low);
    }
  }
  printf("Expected end: %d\n", keycodes_length);
  printf("end\n");
}

/* context-free functions */
char _keysym_to_char(char *keysym) {
  int i;

  if (keysym == NULL)
    return -1;

  /* keysymcharmap comes from xdo_util.h */
  for (i = 0; keysymcharmap[i].keysym; i++) {
    if (!strcmp(keysymcharmap[i].keysym, keysym))
      return keysymcharmap[i].key;
  }

  if (strlen(keysym) == 1)
    return keysym[0];

  return -1;
}

/* main test */
int main() {
  char *display_name;
  xdo_t *xdo;

  char *yay;

  if ( (display_name = getenv("DISPLAY")) == (void *)NULL) {
    fprintf(stderr, "Error: DISPLAY environment variable not set\n");
    exit(1);
  }

  yay = strdup("ctrl+l");

  xdo = xdo_new(display_name);
  xdo_mousemove(xdo, 100, 100);
  xdo_keysequence(xdo, yay);
  xdo_type(xdo, "ls");
  xdo_keysequence(xdo, "Return");
  xdo_free(xdo);

  return 0;
}
