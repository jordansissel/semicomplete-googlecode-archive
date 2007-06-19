/* xdo utility pieces */

/* human to Keysym string mapping */
char *symbol_map[] = {
  "alt", "Alt_L",
  "ctrl", "Control_L",
  "control", "Control_L",
  "meta", "Meta_L",
  "super", "Super_L",
  "shift", "Shift_L",
  NULL, NULL,
};

/* Map keysym name to actual ascii */
typedef struct keysymcharmap {
  char *keysym;
  char key;
} keysymcharmap_t;

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

/* map character to keycode */
typedef struct charcodemap {
  char key;
  int code;
  int shift;
} charcodemap_t;

