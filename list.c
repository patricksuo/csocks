#include "list.h"

void list_add_tail(struct list_head *head, struct list_node *node) {
	if (head->tail) {
		node->next = NULL;
		node->prev = head->tail;
		head->tail->next = node;
		head->tail = node;
	} else {
		/* empty list*/
		node->next = NULL;
		node->prev = NULL;
		head->head = node;
		head->tail = node;
	}
}

struct list_node *list_del_head(struct list_head *head) {
	struct list_node *orphan;
	orphan = head->head;
	if (!orphan) {
		return NULL;
	}

	if (orphan->next) {
		orphan->next->prev = NULL;
		head->head = orphan->next;
		orphan->next = NULL;
		orphan->prev = NULL;
	} else {
		head->head = NULL;
		head->tail = NULL;
	}
	return orphan;
}
