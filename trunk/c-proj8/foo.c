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
					//printf("Processing %c\n", *c);
					if (*(c + 1) == '+') {
						//printf("\nExpecting an argument\n");
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

void _do_opt(int ch, char *value) {
	printf("do_opt('%c', ", ch);

	if (value != NULL)
		printf("\"%s\"", value);
	else 
		printf("0");

	printf(")\n");
}

void _illegal_opt(int ch) {
	printf("illegal_opt('%c')\n", ch);
}

int main(int argc, char **argv) {
	char **f;
	f = do_opts(argc, argv, "abcdef", _do_opt, _illegal_opt);

	printf("Remaining: ");
	while (*f != NULL) {
		printf("%s ", *f);
		*f++;
	}

	printf("\n");
}

