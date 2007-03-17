#include <stdio.h>

char **do_opts(int argc, char **argv, char *control,
					void (*do_opt)(int ch, char *value),
					void (*illegal_opt)(int ch));

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

