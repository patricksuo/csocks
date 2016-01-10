#include "task.h"
#include "hashmap.h"
#include "malloc.h"
//#include "list.h"
#include "log.h"
#include "net.h"

#include <signal.h> /* sigemptyset */
#include <assert.h> /* assert */
#include<stdio.h> /*stderr printf*/

static struct cs_task_t *init_task0();
static void trampoline_wraper(int tid);
static int get_task_id();
static void task_free(struct cs_task_t *task);
static void task_run(struct cs_task_t *task);
static void task_print(struct cs_task_t* task, const char *msg);

static int g_ntask; /* user task count */
static struct cs_task_t *task0; /*scheduler task*/
static struct cs_task_t *task_running; /*current task*/

LIST_HEAD(g_runnable_tasks); /* all runnable task link list */

static struct hashmap_t *task_ctx_map; /*task id -> task pointer*/
//static struct hashmap_t *task_polling_map; /* blocking fd -> task pointer */


static void trampoline_wraper(int tid) {
	struct cs_task_t *task;
	task = hashmap_get(task_ctx_map, &tid, sizeof(int));
	task->startfn(task->startarg);
	task_exit();
}

void task_init(int cap) {
	task_ctx_map = hashmap_new(cap);
}

static int get_task_id() {
	/* TODO need robust id manager (handle overflow, recycle id etc)*/
	static int task_id;
	return ++task_id;
}

static struct cs_task_t *init_task0() {
	struct cs_task_t *task = cs_malloc(sizeof(struct cs_task_t));
	ucontext_t *ctx = cs_malloc(sizeof(ucontext_t));

	//ctx->uc_stack.ss_sp = stack;
	//ctx->uc_stack.ss_size = STACK_SIZE;
	//ctx->uc_link = NULL;
	//sigemptyset(&(ctx->uc_sigmask));

	task->ctx = ctx;
	task->status = READY;
	task->startfn = NULL;
	task->startarg = NULL;
	task->schedulable = 1;

	if (getcontext(ctx) == -1) {
		log("getcontext failed");
		exit(EXIT_FAILURE);
	}
	return task;
}


int task_schedule_loop() {
	assert(task0 == NULL);
	task0 = init_task0();
	cs_net_init();

	struct cs_task_t *task;
	struct list_node *node;
	int ntask = 0;
	int timeout;
	while (1) {
		ntask = list_len(&g_runnable_tasks);
		while (ntask > 0) {
			task_running = task0;
			node = list_del_head(&g_runnable_tasks);
			if (!node) {
				break; /* no more runnable task */
			}
			ntask--;
			task = LIST_ENTY(node, struct cs_task_t, task_list);

			switch (task->status) {
				case EXITING:
					/* do clean up jobs */
					task_print(task, "free task");
					task_free(task);
					if (hashmap_elem_num(task_ctx_map) == 0) {
						return 0; /* no more task*/
					}
					continue;
				case READY:
					/* run task */
					task_print(task, "switch task");
					task_run(task);
					if (task->schedulable) {
						task_print(task, "reschedule task");
						list_add_tail(&g_runnable_tasks, &task->task_list);
					}
					continue;
				case POLLING:
				default:
					continue;
			}
		}
		ntask = list_len(&g_runnable_tasks);
		timeout = ntask > 0 ? 0 : -1; 
		net_poll(timeout);
	}
	return 0;
}

static void task_free(struct cs_task_t *task) {
	assert(task_running == task0); 
	void *datap = NULL;
	g_ntask--;
	cs_free(task->ctx->uc_stack.ss_sp);
	cs_free(task->ctx);
	hashmap_delete(task_ctx_map, &(task->id), sizeof(int), &datap);
	cs_free(task);
}

struct cs_task_t *task_new(void (*fn)(void*), void *arg) {
	char *stack = cs_malloc(STACK_SIZE);
	struct cs_task_t *task = cs_malloc(sizeof(struct cs_task_t));

	ucontext_t *ctx = cs_malloc(sizeof(ucontext_t));
	getcontext(ctx);
	sigemptyset(&(ctx->uc_sigmask));
	ctx->uc_stack.ss_sp = stack;
	ctx->uc_stack.ss_size = STACK_SIZE;
	ctx->uc_link = NULL;

	task->id = get_task_id();
	task->ctx = ctx;
	task->status = READY;
	task->startfn = fn;
	task->startarg = arg;
	task->schedulable = 1;

	makecontext(task->ctx, (void (*)(void))trampoline_wraper, 1, task->id);
	void *old = NULL;
	hashmap_upsert(task_ctx_map, &(task->id),sizeof(int), task, &old);
	assert(old==NULL);  /*old should be NULL; */
	list_add_tail(&g_runnable_tasks, &task->task_list);

	g_ntask++;
	return task;
}

void task_yield(int schedulable) {
	/* can't call yield from scheduler */
	assert(task_running != task0); 
	assert(task_running->schedulable);
	task_running->schedulable = schedulable;
	swapcontext(task_running->ctx, task0->ctx);
}

void task_exit() {
	task_running->status = EXITING;
	task_yield(1);
}

static void task_run(struct cs_task_t *task) {
	assert(task_running == task0); 
	assert(task != task0);
	task_running = task;
	//assert(task->status == READY);
	//task->status = RUNNING
	swapcontext(task0->ctx, task->ctx);
}

static void task_print(struct cs_task_t* task, const char *msg) {
	fprintf(stderr, "%s task_id %d, ctxp %p, status %d, startfn %p, startarg %p \n",
			msg, task->id, task->ctx, task->status,
			task->startfn, task->startarg);
}

struct cs_task_t *task_current() {
	return task_running;
}

void task_rejoin(struct cs_task_t *task) {
	if (task->schedulable) {
		return;
	}
	task->schedulable = 1;
	list_add_tail(&g_runnable_tasks, &task->task_list);
}
