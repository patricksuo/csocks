#ifndef _CS_TASK_H_
#define _CS_TASK_H_
#include <ucontext.h>

#include "list.h"

#define STACK_SIZE 8 * 1024

struct cs_task_t;

typedef enum {
	POLLING,
	READY,
	EXITING,
	/*DNS_LOOKUP,*/
} task_status_t;

struct cs_task_t {
	int id;
	ucontext_t *ctx;
	task_status_t status;
	void (*startfn)(void*); //bootstrap function
	void *startarg;
	struct list_node task_list; 
};

extern int task_schedule_loop();
extern void task_new(void (*fn)(void*), void *arg);
extern void task_yield();
extern void task_exit();
extern void task_init(int task_cap);

#endif /* _CS_TASK_H_ */
