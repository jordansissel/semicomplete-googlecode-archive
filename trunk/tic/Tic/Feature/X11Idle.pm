#!/usr/bin/perl 
# $Id$

package Tic::Feature::X11Idle;

use strict;
use Inline C => << "CODE", INC => '-I/usr/X11R6/include', LIBS => '-L/usr/X11R6/lib -lX11 -lXss -lXext';

#include <X11/Xlib.h>
#include <X11/extensions/scrnsaver.h>
#include <stdio.h>

static Display *dpy;
static Window rootwin;

void foo() {
	printf("hello world\\n");
}

int XConnect(char* display) {
	if (dpy == NULL) {
		if ( (dpy = XOpenDisplay(display)) == NULL) {
			fprintf(stderr, "Error: Can't open display: %s\\n", display);
			return 0;
		}
		rootwin = XDefaultRootWindow(dpy);
	}
	return 1;
}

int XGetIdle() {
	static XScreenSaverInfo *mit_info = NULL;
	int event_base, error_base;
	int idle = 0;

	if (XScreenSaverQueryExtension(dpy, &event_base, &error_base)) {
		if (mit_info == NULL) {
			mit_info = XScreenSaverAllocInfo();
		}
		XScreenSaverQueryInfo(dpy, rootwin, mit_info);
		idle = (mit_info->idle) / 1000;
	} else {
		fprintf(stderr, "XScreenSaverExtension not available on this display\\n");
	}

	return idle;
}

CODE

print __PACKAGE__ . " loaded.\n";

1;
