#ifndef _CS_LIST_H_
#define _CS_LIST_H_

#include <stddef.h> /* offsetof */

struct list_node;

#ifndef NULL
#define NULL 0
#endif

struct list_node {
	struct list_node *prev, *next;
};

struct list_head {
	struct list_node *head, *tail;
};

#define LIST_HEAD(name) \
	struct list_head name = {0, 0}


/**
 * LIST_ENTRY - get the struct for this entry
 * @ptr:        the &struct list_head pointer.
 * @type:       the type of the struct this is embedded in.
 * @member:     the name of the list_head within the struct.
 */
#define LIST_ENTY(ptr, type, member) ({ \
		const typeof( ((type *)0)->member ) *__mptr = (ptr); \
		(type *)( (char *)__mptr - offsetof(type,member) );})


extern void list_add_tail(struct list_head *head, struct list_node *node);
extern struct list_node *list_del_head(struct list_head *head);


#endif /* _CS_LIST_H_ */
