/* xquerytree 
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
  NULL, NULL,
};

static Display *xdpy;

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

  return main_loop();
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
  int i;
  int len;
  char key[2];
  memset(key, 0, 2);

  len = strlen(args);

  for (i = 0; i < len; i++) {
    int keycode;
    key[0] = *(args + i);
    keycode = XKeysymToKeycode(xdpy, XStringToKeysym(key));
    XTestFakeKeyEvent(xdpy, keycode, True, CurrentTime);
    XTestFakeKeyEvent(xdpy, keycode, False, CurrentTime);
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

  keys = malloc(keys_len * sizeof(int));

  if (strcspn(args, " \t\n.-[]{}\\|") != strlen(args)) {
    fprintf(stderr, "Error: Invalid key sequence '%s'\n", args);
    return;
  }

  tok = strtok_r(args, "+", &tokctx);
  while (tok != NULL) {
    /* Lowercase it */
    //for (i = 0; tok[i] != (char)NULL; i++)
      //tok[i] = tolower(tok[i]);

    printf("key: %s\n", tok);
    keys[nkeys] = XKeysymToKeycode(xdpy, XStringToKeysym(tok));
    if (keys[nkeys] == 0) {
      fprintf(stderr, "No such key name '%s'. Ignoring it.\n", tok);
      continue;
    }

    nkeys++;
    if (nkeys == keys_len) {
      keys_len *= 2;
      keys = realloc(keys, keys_len);
    }
    
    tok = strtok_r(NULL, "+", &tokctx);
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
