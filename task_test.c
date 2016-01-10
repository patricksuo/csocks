#include<stdio.h>

#include "sock5.h"
#include "task.h"
#include "malloc.h"
#include "utils.h"

void task_ping(void *arg) {
	int ntime = *(int*)arg;
	//int id = ntime;
	//printf("task_ping start %d ntime\n", ntime);
	while (ntime)  {
		ntime--;
		task_yield(1);
	}
	//printf("task_ping %d end \n", id );
}

void main_loop(void *arg) {
	int ntime = *(int*)arg;
	while (ntime) {
		ntime--;
		int *arg = cs_malloc(sizeof(int));
		*arg = 3;
		task_new(task_ping, arg);
		if (ntime % 5 == 0) {
			task_yield(1);
		}
	}
}


int main(int argc, char **argv) {
	printf("hello world %x\n", S5_VERSION);
	task_init(1024);
	print_stats();
	int i = 0;
	for (i=1; i<1050; i++) {
		int *arg = cs_malloc(sizeof(int));
		*arg = i;
		task_new(task_ping, arg);
	}
	int *arg = cs_malloc(sizeof(int));
	*arg = 3500;
	task_new(main_loop, arg);
	task_schedule_loop();
	print_stats();
	return 0;
}
