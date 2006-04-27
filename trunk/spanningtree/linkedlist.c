/*
 * linked list implementation
 */

#include <stdio.h>

#include "linkedlist.h"

void lladd(llnode_t **list, void *data) {
	llnode_t *node = malloc(sizeof(llnode_t));

	node->data = data;
	node->next = *list;
	if (*list == NULL) {
		/* Create first entry */
		node->tail = node;
		node->head = node;
	} else {
		llnode_t *h;
		node->tail = (*list)->tail;

		if (*list == (*list)->head) {
			/* Fix up head locations */
			for (h = *list; h != NULL; h = h->next)
				h->head = node;
			node->head = node;
		} else {
			node->head = (*list)->head;
		}
	}

	*list = node;
}

void llmerge(llnode_t **x, llnode_t **y) {
	if (*x) {
		if (*y) {
			(*x)->tail->next = *y;
			(*x)->tail = (*y)->tail;
			(*y)->head = (*x)->head;
		}
		*y = *x;
	} else if (*y) {
		//(*y)->tail->next = *x;
		*x = *y;
	}
}

void printlist(llnode_t *list, void (*func)(void *)) {
	llnode_t *t = list;
	int x = 0;
	while (t != NULL) {
		func(t->data);
		printf(", ");
		//printf("%s, ", t->data);
		t = t->next;
		if (x++ > 8) break;
	}

	if (list != NULL) {
		printf("[Tail: "); 
		func(list->tail->data);
		printf(" / Head: ");
		func(list->head->data);
		printf("]");
	}

	printf("\n");
}

#ifdef TEST
void printnode(void *d) {
	printf("%s", d);
}

int main(int argc, char **argv) {
	llnode_t *x = NULL;
	llnode_t *y = NULL;
	int opt;

	/*
	while (--argc > 0) {
		argv++;
		lladd(&ll, *argv);
		printlist(ll);
	}
	*/

	//lladd(&x, "one");
	if (argc < 2) { printf("Bad args\n"); return 0; }

	opt = atoi(*++argv);

	switch (opt) {
		case 1:
			lladd(&y, "hello");
			lladd(&y, "hello");
			lladd(&y, "hello");
			llmerge(&x,&y);
			printlist(x, &printnode);
			printlist(y, &printnode);

			x = y = NULL;
			lladd(&x, "Hi");
			lladd(&x, "Hi");
			lladd(&x, "Hi");
			llmerge(&x, &y);

			printlist(x, &printnode);
			printlist(y, &printnode);
			break;
		case 2:
			lladd(&x, "one"); printf("Head: %08x\n", x->head);
			lladd(&x, "two"); printf("Head: %08x\n", x->head);
			lladd(&x, "three"); printf("Head: %08x\n", x->head);
			lladd(&x, "four"); printf("Head: %08x\n", x->head);
			lladd(&x, "five"); printf("Head: %08x\n", x->head);
			lladd(&(x->next), "HELLO");printf("Head: %08x\n", x->head);
			lladd(&(x->next->next->next), "THERE");printf("Head: %08x\n", x->head);
			lladd(&(x->next->next->next), "THERE");printf("Head: %08x\n", x->head);
			printlist(x, &printnode);
			printlist(x->next->next, &printnode);
			break;
		case 3:
			lladd(&x, "one");
			lladd(&x, "two");
			lladd(&y, "hello");
			lladd(&y, "there");
			lladd(&y, "yes");
			llmerge(&(x->next->next), &y);
			printlist(x, &printnode);
	}

}
#endif /* TEST */
