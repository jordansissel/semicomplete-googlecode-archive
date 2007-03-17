/*
 * $Id$
 */

#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>

typedef struct {
	Display *dpy;
} XINFO;

Bool ProcessEvent(Display *dpy, XINFO *info);
Bool ProcessMotionNotify(Display *dpy, XINFO *info, XMotionEvent *pEv);
Bool ProcessButtonPress(Display *dpy, XINFO *info, XButtonEvent *pEv);
Bool ProcessButtonRelease(Display *dpy, XINFO *info, XButtonEvent *pEv);

/* Stolen from x2x */
#define XSAVECONTEXT(A, B, C, D) XSaveContext(A, B, C, (XPointer)(D))
#define XFINDCONTEXT(A, B, C, D) XFindContext(A, B, C, (XPointer *)(D))

#define MIDDLE_MOUSE 2

int main (int argc, char **argv) {
	Display        *dpy;
	char           *pcdisplay;
	Window          trigger,
	                root;
	Screen         *screen;
	XINFO           info;
	XSetWindowAttributes xswa;

	Bool            pending;
	int             nfds;
	fd_set          fdset;
	int             xconn;

	if ( (pcdisplay = getenv("DISPLAY")) == NULL) {
		fprintf(stderr, "Error: DISPLAY environment variable not set\n");
		exit(1);
	}

	printf("Display: %s\n", pcdisplay);

	if ( (dpy = XOpenDisplay(pcdisplay)) == NULL) {
		fprintf(stderr, "Error: Can't open display: %s", pcdisplay);
		exit(1);
	}

	screen = XDefaultScreenOfDisplay(dpy);

	xswa.override_redirect = True;
	xswa.background_pixel = XBlackPixelOfScreen(screen);

	trigger = XCreateWindow(dpy, XDefaultRootWindow(dpy), -1, -1, 1, 1,
											0, 0, InputOutput, 0, 
											CWBackPixel | CWOverrideRedirect, &xswa);


	XMapRaised(dpy, trigger);

	XFlush(dpy);
	sleep(1);
	
	XSelectInput(dpy, trigger, 
					 PointerMotionMask | ButtonPressMask | ButtonReleaseMask);

	XSAVECONTEXT(dpy, trigger, MotionNotify,         ProcessMotionNotify);
	XSAVECONTEXT(dpy, trigger, ButtonPress,          ProcessButtonPress);
	XSAVECONTEXT(dpy, trigger, ButtonRelease,        ProcessButtonRelease);

	printf("XGrabPointer() returned: %d\n", 
	XGrabPointer(dpy, trigger, True,
					 PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
					 GrabModeAsync, GrabModeAsync, None, None, CurrentTime)
			);
	XFlush(dpy);

	nfds = getdtablesize();
	xconn = XConnectionNumber(dpy);

	root = XDefaultRootWindow(dpy);

	while (True) {
		XQueryPointer(dpy, root)
		ProcessEvent(dpy, &info);
	}

	sleep(3);

	XUngrabPointer(dpy, CurrentTime);
	XDestroyWindow(dpy, trigger);
	XFlush(dpy);
	XCloseDisplay(dpy);

	return 0;
}


Bool ProcessEvent(Display *dpy, XINFO *info) {
	XEvent ev;
	XAnyEvent *pev = (XAnyEvent *)&ev;
	int (*handler)();;

	XNextEvent(dpy, &ev);
	handler = 0;

	if ((!XFINDCONTEXT(dpy, pev->window, pev->type, &handler)) ||
		 (!XFINDCONTEXT(dpy, None, pev->type, &handler))) {
		return ((*handler)(dpy, info, &ev));
	} else {
		printf("No handler for window 0x%x, event type %d\n", 
				 pev->window, pev->type);
	}

	return False;

}

Bool ProcessMotionNotify(Display *dpy, XINFO *info, XMotionEvent *pEv) {
	printf("Motion!\n");
}

Bool ProcessButtonPress(Display *dpy, XINFO *info, XButtonEvent *pEv) {
	printf("Press! Button: %d (Mask: 0x%x)\n", pEv->button, pEv->state);
}

Bool ProcessButtonRelease(Display *dpy, XINFO *info, XButtonEvent *pEv) {
	printf("Release! Button: %d (Mask: 0x%x)\n", pEv->button, pEv->state);
}
