
#include <stdio.h>
#include <stdlib.h>
#include "xdo.h"

void cmd_mousemove(int argc, char **args);
void cmd_mousedown(int argc, char **args);
void cmd_mouseup(int argc, char **args);
void cmd_click(int argc, char **args);
void cmd_type(int argc, char **args);
void cmd_key(int argc, char **args);

xdo_t *xdo;

struct dispatch {
  const char *name;
  void (*func)(int argc, char **args);
} dispatch[] = {
  "mousemove", cmd_mousemove,
  "mousedown", cmd_mousedown,
  "mouseup", cmd_mouseup,
  "click", cmd_click,
  "type", cmd_type,
  "key", cmd_key,
  NULL, NULL,
};

int main(int argc, char **argv) {
  char *cmd;
  int i;
  xdo = xdo_new(getenv("DISPLAY"));

  if (argc < 2) {
    fprintf(stderr, "usage: %s <cmd> <args>\n", argv[0]);
    exit(1);
  }

  cmd = *++argv; /* argv[1] */
  argc -= 2;

  argv++; /* skip 'cmd' */
  for (i = 0; dispatch[i].name != NULL; i++) {
    if (!strcasecmp(dispatch[i].name, cmd)) {
      dispatch[i].func(argc, argv);
      break;
    }
  }

  xdo_free(xdo);
}


void cmd_mousemove(int argc, char **args) {
  int x, y;

  if (argc != 2) {
    fprintf(stderr, "usage: move <xcoord> <ycoord>\n");
    fprintf(stderr, "You specified the wrong number of args.\n");
    return;
  }

  x = atoi(args[0]);
  y = atoi(args[1]);

  xdo_mousemove(xdo, x, y);
}

void cmd_mousedown(int argc, char **args) {
  int button;

  if (argc != 1) {
    fprintf(stderr, "usage: mousedown <button>\n");
    fprintf(stderr, "You specified the wrong number of args.\n");
    return;
  }

  button = atoi(args[0]);

  xdo_mousedown(xdo, button);
}

void cmd_mouseup(int argc, char **args) {
  int button;

  if (argc != 1) {
    fprintf(stderr, "usage: mousedown <button>\n");
    fprintf(stderr, "You specified the wrong number of args.\n");
    return;
  }

  button = atoi(args[0]);
  printf("button %s / %d\n", args[0], button);

  xdo_mouseup(xdo, button);
}

void cmd_click(int argc, char **args) {
  cmd_mousedown(argc, args);
  cmd_mouseup(argc, args);
}

void cmd_type(int argc, char **args) {
  int i;

  if (argc == 0) {
    fprintf(stderr, "usage: type <things to type>\n");
    fprintf(stderr, "You specified the wrong number of args.\n");
    return;
  }

  for (i = 0; i < argc; i++) {
    xdo_type(xdo, args[i]);
  }
}

void cmd_key(int argc, char **args) {
  int i;

  if (argc == 0) {
    fprintf(stderr, "usage: move <keyseq1> [keyseq2 ... keyseqN]\n");
    fprintf(stderr, "You specified the wrong number of args.\n");
    return;
  }

  for (i = 0; i < argc; i++) {
    xdo_keysequence(xdo, args[i]);
  }
}

