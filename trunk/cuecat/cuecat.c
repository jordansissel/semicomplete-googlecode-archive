/*
 *
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>

#define XK_MISCELLANY

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysymdef.h>

int fatal(Display *d, XErrorEvent *e) {
	char error[120];
	XGetErrorText(d, e->error_code, error, 120);

	fprintf(stderr, "Error: [%d] %s\n", e->error_code, error);

	exit(1);

	return 0;
}

int fatal2(Display *d, int e, const char *name) {
	char error[120];
	XGetErrorText(d, e, error, 120);

	fprintf(stderr, "Error: (%s) %s [Code %d]\n", name, error, e);

	exit(1);

	return 0;
}

int main(int argc, char **argv) {
	Display *dpy;
	char *pcdisplay;
	Window root;
	int ret;
	char *input = NULL;
	int inputlen = 128;
	int inputoffset = 0;

	input = (char *)malloc(inputlen);

	if ((pcdisplay = getenv("DISPLAY")) == NULL) {
		fprintf(stderr, "Error: DISPLAY environment variable not set\n");
		exit(1);
	}

	if ( (dpy = XOpenDisplay(pcdisplay)) == NULL) {
		fprintf(stderr, "Error: Can't open display: %s\n", pcdisplay);
		exit(1);
	}
	printf("Opened display: %s\n", XDisplayName(pcdisplay));

	root = DefaultRootWindow(dpy);

	{/* 
		Window retroot, child;
		int xc, yc, winx, winy, maskret;
		XQueryPointer(dpy, root, &retroot, &child, &xc, &yc, &winx, &winy, &maskret);
		printf("Query: (%d,%d)\n", xc, yc);
	*/}

	/*
	ret = XAllowEvents(dpy, AsyncBoth, CurrentTime);
	if (ret) {
		fatal2(dpy, ret, "XAllowEvents");
	}
	*/

	XSelectInput(dpy, root, KeyPressMask);

	XGrabKey(dpy, XKeysymToKeycode(dpy, XK_F10), AnyModifier, 
						root, True, GrabModeAsync, GrabModeAsync);


	for (;;) {
		XEvent e;
		XKeyEvent ke = e.xkey;
		if (XPending(dpy)) {
			char keyname[10];
			XNextEvent(dpy, &e);
			strcpy(keyname, XKeysymToString(XKeycodeToKeysym(dpy, ke.keycode, 0)));

			if (!strcmp(keyname, "F10")) {
				XGrabKeyboard(dpy, root, True, GrabModeSync, GrabModeSync, CurrentTime);
				inputoffset = 0;
				memset(input, 0, inputlen);
			} else if (!strcmp(keyname, "Return")) {
				printf("Input: '%s'\n", input);
				XUngrabKeyboard(dpy, CurrentTime);
			} else {
				if (strlen(keyname) == 1) {
					*(input + inputoffset++) = keyname[0];
				}
			}


		}
	}

}
