/* xquerytree 
 */

#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/XTest.h>

int main() {
  Display *xdpy;
  Window root;
  char *display_name = NULL;
  int ver;

  if ( (xdpy = XOpenDisplay(display_name)) == NULL) {
    fprintf(stderr, "Error: Can't open display: %s", display_name);
    exit(1);
  }

  if (XTestQueryExtension(xdpy, &ver, &ver, &ver, &ver) != True) {
    printf("No xtest :(\n");
    return 1;
  }

  root = XDefaultRootWindow(xdpy);

  {
    int control, alt, key_l, key_two;
    control = XKeysymToKeycode(xdpy, XStringToKeysym("Control_L"));
    alt = XKeysymToKeycode(xdpy, XStringToKeysym("Alt_L"));
    key_l = XKeysymToKeycode(xdpy, XStringToKeysym("L"));
    key_two = XKeysymToKeycode(xdpy, XStringToKeysym("2"));

    printf("%d %d %d %d\n", control, alt, key_l, key_two);

    sleep(2);
    XTestFakeButtonEvent(xdpy, 1, True, CurrentTime); // button down
    XTestFakeButtonEvent(xdpy, 1, False, CurrentTime); // button release
    XTestFakeMotionEvent(xdpy, 0, 100, 100, CurrentTime);
    XTestFakeKeyEvent(xdpy, key_l, True, CurrentTime);
    XTestFakeKeyEvent(xdpy, key_l, False, CurrentTime);
    //XTestFakeKeyEvent(xdpy, alt, True, CurrentTime);
    //XTestFakeKeyEvent(xdpy, key_two, True, CurrentTime);
    //XTestFakeKeyEvent(xdpy, key_two, False, CurrentTime);
    //XTestFakeKeyEvent(xdpy, alt, False, CurrentTime);
  }

  return 0;
}

