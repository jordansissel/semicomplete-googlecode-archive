/*
 *
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>

#define XK_MISCELLANY
#define XK_LATIN1

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

	{ /*
		Window retroot, child;
		int xc, yc, winx, winy, maskret;
		XQueryPointer(dpy, root, &retroot, &child, &xc, &yc, &winx, &winy, &maskret);
		printf("Query: (%d,%d)\n", xc, yc);
	*/ }

	ret = XAllowEvents(dpy, AsyncBoth, CurrentTime);
	/*
	if (ret) {
		fatal2(dpy, ret, "XAllowEvents");
	}
	*/

	XSelectInput(dpy, root, KeyPressMask);

	XGrabKey(dpy, XKeysymToKeycode(dpy, XK_F10), AnyModifier, 
						root, True, GrabModeSync, GrabModeSync);


	for (;;) {
		XEvent e;
		XKeyEvent ke;
		if (XPending(dpy)) {
			char *kp;
			XKeyEvent ke = e.xkey;
			int keysym;
				
			XNextEvent(dpy, &e);
			ke = e.xkey;

			if (ke.type == KeyPress) {
				keysym = XKeycodeToKeysym(dpy, ke.keycode, 0);
				kp = XKeysymToString(keysym);
				//strcpy(keyname, XKeysymToString(XKeycodeToKeysym(dpy, ke.keycode, 0)));
				printf("[%d] Sym: %d / %d\n", ke.type, keysym, XK_F10);
				if (kp != NULL) {
					printf("Name: %s\n", kp);
				}

				switch (keysym) {
					case XK_F10:
						XGrabKeyboard(dpy, root, True, GrabModeAsync, GrabModeAsync, CurrentTime);
						inputoffset = 0;
						memset(input, 0, inputlen);
						break;
					case XK_Return:
						printf("Input: '%s'\n", input);
						XUngrabKeyboard(dpy, CurrentTime);
						break;
					case XK_period:
						*(input + inputoffset++) = '.';
					default:
						if ((kp != NULL) && (strlen(kp) == 1)) {
							*(input + inputoffset++) = kp[0];
						}
						break;
				}
			}
		}
	}

}
