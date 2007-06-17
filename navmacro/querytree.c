/* xquerytree 
 */

#include <X11/Xlib.h>

int main() {
  Display *xdisplay;
  Window root;
  char *display_name = ":0";

  int i;
  Window *children, dummy;
  unsigned int nchildren;

  if ( (dpy = XOpenDisplay(pcDisplay)) == NULL) {
    fprintf(stderr, "Error: Can't open display: %s", pcDisplay);
    exit(1);
  }

  root = XDefaultRootWindow(dpy);
  //XGetWindowAttributes(dpy, root, &attr);

  if (!XQueryTree(xdisplay, root, &dummy, &dummy, &children, &nchildren))
    return 1;

  for (i = 0; i < nchildren; i++) {
    Window w = children[i];
    printf("%d\n", w);
  }

  return 0;
}
