/* xdo
 *
 * "do" things normally done from the keyboard and mouse
 *
 * Implemented:
 * type something_to_type
 * move xcoord ycoord [screennum]
 * key key_sequence
 * sleep sleep_in_ms
 *
 * Commands a separated by semicolon and/or newlines.
 *
 * Example:
 *
 * Hello world using faked keyboard input
 * echo 'type hello world' | ./xdo
 *
 * Use this while firefox is focused to go to google:
 * echo 'key ctrl+l; sleep 1; type http://www.google.com/; key Return' | ./xdo
 *   - This does:
 *     1) Ctrl+L to focus the URL bar
 *     2) Sleep to let the event propogate
 *     3) Type http://www.google.com/
 *     4) Press return
 */

#include <sys/select.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/XTest.h>

int main_loop();
void xdo(char *cmd);
int get_keycode(char key);
int get_shift(char key);
void populate_charcode_map();
char char_from_keystring(char *keysym);

/* Commands */
void cmd_type(char *args);
void cmd_move(char *args);
void cmd_key(char *args);
void cmd_keydown(char *args);
void cmd_keyup(char *args);
void cmd_click(char *args);
void cmd_mouseup(char *args);
void cmd_mousedown(char *args);
void cmd_sleep(char *args);

typedef struct dispatch {
  char *name;
  void (*func)(char *args);
} dispatch_t;

static dispatch_t commands[] = {
  "type", cmd_type,
  "key", cmd_key,
  "sleep", cmd_sleep,
  "move", cmd_move,
  NULL, NULL,
};

static char *symbol_map[] = {
  "alt", "Alt_L",
  "ctrl", "Control_L",
  "control", "Control_L",
  "meta", "Meta_L",
  "super", "Super_L",
  "shift", "Shift_L",
  NULL, NULL,
};

typedef struct charcodemap {
  char key;
  int code;
  int shift;
} charcodemap_t;

typedef struct keysymcharmap {
  char *keysym;
  char key;
} keysymcharmap_t;

static charcodemap_t *charcodes = NULL;
static int keycode_lowest;
static int keycode_highest;
static Display *xdpy = NULL;

keysymcharmap_t keysymcharmap[] = {
  "Return", '\n',
  "ampersand", '&',
  "apostrophe", '\'',
  "asciicircum", '^',
  "asciitilde", '~',
  "asterisk", '*',
  "at", '@',
  "backslash", '\\',
  "bar", '|',
  "braceleft", '{',
  "braceright", '}',
  "bracketleft", '[',
  "bracketright", ']',
  "colon", ':',
  "comma", ',',
  "dollar", '$',
  "equal", '=',
  "exclam", '!',
  "grave", '`',
  "greater", '>',
  "less", '<',
  "minus", '-',
  "numbersign", '#',
  "parenleft", '(',
  "parenright", ')',
  "percent", '%',
  "period", '.',
  "plus", '+',
  "question", '?',
  "quotedbl", '"',
  "semicolon", ';',
  "slash", '/',
  "space", ' ',
  "tab", '\t',
  "underscore", '_',
  NULL, 0,
};

int main() {
  char *display_name = NULL;
  int dummy;
  
  if ( (display_name = getenv("DISPLAY")) == (void *)NULL) {
    fprintf(stderr, "Error: DISPLAY environment variable not set\n");
    exit(1);
  }

  if ((xdpy = XOpenDisplay(display_name)) == NULL) {
    fprintf(stderr, "Error: Can't open display: %s", display_name);
    exit(1);
  }

  if (XTestQueryExtension(xdpy, &dummy, &dummy, &dummy, &dummy) != True) {
    fprintf(stderr, "Error: No XTEST extension available on %s", display_name);
    return 1;
  }

  populate_charcode_map();

  return main_loop();
} 

void populate_charcode_map() {
  int key_low, key_high;
  int dummy;
  int i;
  int charcodes_len;

  XDisplayKeycodes(xdpy, &key_low, &key_high);

  /* Double in size because there's actually 2 characters per key
   * shifted and nonshifted. */
  charcodes_len = (key_high - key_low) * 2;
  charcodes = malloc(charcodes_len * sizeof(charcodemap_t));
  memset(charcodes, 0, charcodes_len * sizeof(charcodemap_t));

  int shiftcode = XKeysymToKeycode(xdpy, XStringToKeysym("Shift_L"));

  keycode_lowest = key_low;
  keycode_highest = key_high;

  //printf("Low/High: %d/%d\n", key_low, key_high);
  for (i = key_low; i <= key_high; i++) {
    char *keybuf;
    int j;

    for (j = 0; j <= 1; j++) {
      int index = (i * 2 + j) - key_low;
      keybuf = XKeysymToString(XKeycodeToKeysym(xdpy, i, j));
      if (keybuf != NULL) {
        //printf("%d => %s\n", i, keybuf);
        charcodes[index].key = char_from_keystring(keybuf);
      } else {
        //printf("No such key for code %d\n", i);
        charcodes[index].key = -1;
      }
      charcodes[index].code = i;
      charcodes[index].shift = j ? shiftcode : 0;
    }
  }
  //printf("end: %d\n", i);

}

int get_keycode(char key) {
  int i;
  int len = keycode_highest - keycode_lowest;
  for (i = 0; i < len; i++) {
    //if (charcodes[i].key != -1)
      //printf("%c vs %c\n", charcodes[i].key, key);
    if (charcodes[i].key == key)
      return charcodes[i].code;
  }

  return -1;
}

int get_shift(char key) {
  int i;
  int len = keycode_highest - keycode_lowest;
  for (i = 0; i < len; i++) {
    if (charcodes[i].key == key)
      return charcodes[i].shift;
  }

  return 0;
}

char char_from_keystring(char *keysym) {
  int i;

  for (i = 0; keysymcharmap[i].keysym; i++) {
    if (!strcmp(keysymcharmap[i].keysym, keysym))
      return keysymcharmap[i].key;
  }

  if (strlen(keysym) == 1)
    return keysym[0];

  return -1;
}

int main_loop() {
#define READBUFSIZE (4096)
#define COMMANDSEP (";\n")

  char buf[READBUFSIZE];
  int bytes = 0;
  int rem_bytes = 0;
  char *tok;
  char *tokctx;

  memset(buf, 0, READBUFSIZE);

  while (!feof(stdin)) {
    bytes = fread(buf+rem_bytes, 1, READBUFSIZE - rem_bytes, stdin);
    rem_bytes = bytes;
    tok = strtok_r(buf, COMMANDSEP, &tokctx);
    while (tok != NULL) {
      xdo(tok);
      rem_bytes -= strlen(tok);
      tok = strtok_r(NULL, COMMANDSEP, &tokctx);
    }
  }

  return 0;
}

void xdo(char *cmd) {
  char *tok = NULL;
  char *tokctx = NULL;

  int i;
  tok = strtok_r(cmd, " ", &tokctx);
  for (i = 0; i < sizeof(commands) && commands[i].name != NULL; i++) {
    if (!strcmp(commands[i].name, tok))
      commands[i].func(tokctx);
  }
}

void cmd_type(char *args) {
  int i = 0;
  int len = 0;
  char key = 0;
  int keycode = 0;
  int shift = 0;

  len = strlen(args);

  //printf("Typing: %s\n", args);
  for (i = 0; i < len; i++) {
    key = *(args + i);
    keycode = get_keycode(key);
    shift = get_shift(key);
    //printf("K: %d (%s)\n", keycode, (shift ? "shifted" : ""));
    if (shift)
      XTestFakeKeyEvent(xdpy, shift, True, CurrentTime);

    XTestFakeKeyEvent(xdpy, keycode, True, CurrentTime);
    XTestFakeKeyEvent(xdpy, keycode, False, CurrentTime);

    if (shift)
      XTestFakeKeyEvent(xdpy, shift, False, CurrentTime);
    XFlush(xdpy);
  }
}

void cmd_key(char *args) {
  char *tokctx;
  char *tok;
  int i;
  int *keys = NULL;
  int nkeys = 0;
  int keys_len = 10;
  char *strptr = NULL;

  keys = malloc(keys_len * sizeof(int));

  if (strcspn(args, " \t\n.-[]{}\\|") != strlen(args)) {
    fprintf(stderr, "Error: Invalid key sequence '%s'\n", args);
    return;
  }

  strptr = args;

  while ((tok = strtok_r(strptr, "+", &tokctx)) != NULL) {
    int keysym;
    if (strptr != NULL)
      strptr = NULL;

    /* Check if 'tok' (string keysym) is an alias to another key */
    for (i = 0; symbol_map[i] != NULL; i+=2)
      if (!strcasecmp(tok, symbol_map[i]))
        tok = symbol_map[i + 1];

    keysym = XStringToKeysym(tok);
    if (keysym == NoSymbol) {
      fprintf(stderr, "(symbol) No such key name '%s'. Ignoring it.\n", tok);
      continue;
    }

    keys[nkeys] = XKeysymToKeycode(xdpy, keysym);

    if (keys[nkeys] == 0) {
      fprintf(stderr, "No such key '%s'. Ignoring it.\n", tok);
      continue;
    }

    nkeys++;
    if (nkeys == keys_len) {
      keys_len *= 2;
      keys = realloc(keys, keys_len);
    }
  }

  for (i = 0; i < nkeys; i++)
    XTestFakeKeyEvent(xdpy, keys[i], True, CurrentTime);
  for (i = nkeys - 1; i >= 0; i--)
    XTestFakeKeyEvent(xdpy, keys[i], False, CurrentTime);

  XFlush(xdpy);
}

void cmd_sleep(char *args) {
  unsigned long sleep_ms;
  struct timeval t;

  sleep_ms = strtoul(args, NULL, 0);
  t.tv_sec = sleep_ms / 1000;
  t.tv_usec = (sleep_ms % 1000) * 1000; /* milliseconds -> microseconds */
  select(0, NULL, NULL, NULL, &t);
}

void cmd_move(char *args) {
  int x, y;
  int screen;
  sscanf(args, "%d %d %d", &x, &y, &screen);

  XTestFakeMotionEvent(xdpy, -1, x, y, CurrentTime);

  XFlush(xdpy);
}

void strtolower(char *str) {
  int i;
  for (i = 0; str[i] != (char)NULL; i++)
    str[i] = tolower(str[i]);
}
