#include <stdio.h>

char **do_opts(int argc, char **argv, char *control,
					void (*do_opt)(int ch, char *value),
					void (*illegal_opt)(int ch)) {
	char **pt = argv;
	char *c;
	char found;
	char done;

	/* Skip the 0th argument, the program name */
	pt++;

	while (*pt != NULL && **pt == '-') {
		done = 0;
		while (!done && *(++*pt) != '\0') {
			found = 0;
			for (c = control; *c != '\0' && !found; c++) {
				if (**pt == *c) {
					found = 1;
					if (*(c + 1) == '+') {
						if (*(*pt + 1) != '\0') {
							do_opt(*c, (*pt + 1));
						} else {
							if (*(pt + 1) == NULL) {
								illegal_opt(**pt);
							} else {
								do_opt(*c, *(pt + 1));
								pt++;
							}
						}
						done = 1;
					} else {
						do_opt(*c, NULL);
					}
				}
			}
			if (found == 0) {
				illegal_opt(**pt);
			}
		}
		pt++;
	}

	return pt;
}
