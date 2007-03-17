
#include <stdio.h>
#include <stdlib.h>

#define LEN 100000
#define SIZE (LEN * sizeof(int))
int main() {
	int *foo, *bar;
	int fizz;

	foo = malloc(SIZE);
	memset(foo, 0, SIZE);
	*(foo + LEN + 10) = 42;
	memcpy(&fizz, foo + LEN + 10, sizeof(int));
	printf("%d\n", fizz);
	fprintf(stderr, "Should've died above...\n");
	free(foo);
	bar = malloc(1200);

	free(bar);

	return 0;
}
