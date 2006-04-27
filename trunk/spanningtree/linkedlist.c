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
	if (*x) {
		(*x)->tail->next = *y;
		if (*y)
			(*x)->tail = (*y)->tail;
		*y = *x;
	} else if (*y) {
		(*y)->tail->next = *x;
		//if (*x)
			//(*y)->tail = (*x)->tail;
		*x = *y;
	}
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

	//lladd(&x, "one");

	lladd(&y, "hello");
	llmerge(&x,&y);

	x = y = NULL;
	lladd(&x, "Hi");
	llmerge(&x, &y);

	printlist(x);
	printlist(y);

}
#endif /* TEST */
