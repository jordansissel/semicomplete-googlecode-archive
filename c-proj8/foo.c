#define NULL 0

char **do_opts(int argc, char **argv, char *control,
					void (*do_opt)(int ch, char *value),
					void (*illegal_opt)(int ch)) {
	unsigned char offset;
	char **pt = argv;
	char *c;
	char done;

	/* Skip the 0th argument, the program name */
	pt++;

	while (*pt != NULL && **pt == '-') {
		done = 0;
		offset = 0;
		while (!done && *(*pt + offset) != '\0') {
			char *foo = *pt + offset
			for (c = control; *c != '\0'; c++) {
				if (**pt == *c) {
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
					break;
				}
			}
			if (*c == '\0') {
				illegal_opt(**pt);
			}
		}
		pt++;
	}

	return pt;
}
