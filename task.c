#include "task.h"
#include "hashmap.h"
#include "malloc.h"
//#include "list.h"
#include "log.h"

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
static struct cs_task_t *task_net; /*task for network polling*/
static struct cs_task_t *task_running; /*current task*/

LIST_HEAD(g_tasks_queue); /* all task link list */

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

	if (getcontext(ctx) == -1) {
		log("getcontext failed");
		exit(EXIT_FAILURE);
	}
	return task;
}


int task_schedule_loop() {
	assert(task0 == NULL);
	task0 = init_task0();

	struct cs_task_t *task;
	struct list_node *node;
	int nemptyswitch = 0;
	while (1) {
		task_running = task0;
		if (nemptyswitch > g_ntask) {
			/* 
			 * All user task is blocking,
			 * try net polling task.
			 */
			if (task_net) {
				nemptyswitch = 0;
				//task_print(task_net, "switch to polling task");
				task_run(task_net);
			}
		}
		node = list_del_head(&g_tasks_queue);
		if (!node) {
			return 0; /* no more task*/
		}
		task = LIST_ENTY(node, struct cs_task_t, task_list);

		switch (task->status) {
			case EXITING:
				/* do clean up jobs */
				task_free(task);
				continue;
			case READY:
				/* run task */
				nemptyswitch = 0;
				//task_print(task, "switch task");
				task_run(task);
				list_add_tail(&g_tasks_queue, &task->task_list);
				continue;
			case POLLING:
			default:
				nemptyswitch++; /* no task to run */
				continue;
		}
	}
	return 0;
}

static void task_free(struct cs_task_t *task) {
	assert(task_running == task0); 
	void *datap = NULL;
	g_ntask--;
	cs_free(task->ctx->uc_stack.ss_sp);
	cs_free(task->ctx);
	if (task->startarg) {
		cs_free(task->startarg);
	}
	/* TODO clean up fd */
	hashmap_delete(task_ctx_map, &(task->id), sizeof(int), &datap);
	cs_free(task);
}

void task_new(void (*fn)(void*), void *arg) {
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

	makecontext(task->ctx, (void (*)(void))trampoline_wraper, 1, task->id);
	void *old = NULL;
	hashmap_upsert(task_ctx_map, &(task->id),sizeof(int), task, &old);
	assert(old==NULL);  /*old should be NULL; */
	list_add_tail(&g_tasks_queue, &task->task_list);

	g_ntask++;
}

void task_yield() {
	/* can't call yield from scheduler */
	assert(task_running != task0); 
	swapcontext(task_running->ctx, task0->ctx);
}

void task_exit() {
	task_running->status = EXITING;
	task_yield();
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
