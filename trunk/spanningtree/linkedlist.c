/*
 * linked list implementation
 */

#include <stdio.h>

#include "linkedlist.h"

void lladd(llnode_t **list, void *data) {
	llnode_t *node = malloc(sizeof(llnode_t));

	node->data = data;
	node->next = *list;
	if (*list == NULL)
		node->tail = node;
	else
		node->tail = (*list)->tail;
	*list = node;
}

void llmerge(llnode_t **x, llnode_t **y) {
	if (*y != NULL) {
		(*x)->tail->next = *y;
		(*x)->tail = (*y)->tail;
	}
	*y = *x;
}

#ifdef TEST
void printlist(llnode_t *list) {
	llnode_t *t = list;
	while (t != NULL) {
		printf("%s, ", t->data);
		t = t->next;
	}
	if (list != NULL)
		printf("[Tail: %s]\n", list->tail->data);
	printf("\n");
}

int main(int argc, char **argv) {
	llnode_t *x = NULL;
	llnode_t *y = NULL;

	/*
	while (--argc > 0) {
		argv++;
		lladd(&ll, *argv);
		printlist(ll);
	}
	*/

	lladd(&x, "one");
	lladd(&x, "two");
	lladd(&x, "three");
	lladd(&x, "four");
	lladd(&x, "five");

	//lladd(&y, "hello");
	//lladd(&y, "there");
	//lladd(&y, "how");
	//lladd(&y, "are");
	//lladd(&y, "you");

	printlist(x);
	printlist(y);

	llmerge(&x,&y);
	printlist(x);
	printlist(y);

}
#endif /* TEST */
