
#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
//#include <X11/cursorfont.h>

int main (int argc, char **argv) {
	Display *dpy;
	char *pcdisplay;

	/*XrmInitialize();*/
	pcdisplay = getenv("DISPLAY");
	if (pcdisplay == NULL) {
		fprintf(stderr, "Error: DISPLAY environment variable not set\n");
	}

	printf("Display: %s\n", pcdisplay);

	dpy = XOpenDisplay(pcdisplay);
	if (dpy == NULL) {
		fprintf(stderr, "Error: Can't open display: %s", pcdisplay);
	}

	return 0;
}
