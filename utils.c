#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <execinfo.h>

#include "log.h"

void print_rusage() {
	struct rusage ru;
	if (getrusage(RUSAGE_SELF, &ru)) {
		log("getrusage error");
		return ;
	}
	printf("rusage :\n"
		"maximum resident set size: %ld \n"
		"integral shared memory size: %ld \n"
		"integral unshared data size: %ld \n"
		"integral unshared stack size: %ld \n"
		"page reclaims (soft page faults): %ld \n"
		"page faults (hard page faults): %ld \n"
		"swaps: %ld \n"
		"block input operations: %ld \n"
		"block output operations: %ld \n"
		"IPC messages sent: %ld \n"
		"IPC messages received: %ld \n"
		"signals received: %ld \n"
		"voluntary context switches: %ld \n"
		"involuntary context switches: %ld \n",
		ru.ru_maxrss, ru.ru_ixrss, ru.ru_idrss, ru.ru_isrss,
		ru.ru_minflt, ru.ru_majflt, ru.ru_nswap, ru.ru_inblock,
		ru.ru_oublock, ru.ru_msgsnd, ru.ru_msgrcv, 
		ru.ru_nsignals, ru.ru_nvcsw, ru.ru_nivcsw);
}

void print_stats() {
	int fd ;
	pid_t pid = getpid();
	char path[64];
	char buff[1024];
	sprintf(path, "/proc/%ld/status", (long)pid);
	fd = open(path, O_RDONLY);
	read(fd, buff, 1024);
	printf("%s\n", buff);
}

void print_stack(int fd) {
	if (fd <0) fd = STDERR_FILENO;
	void *buffer[100];
	int nptr;
	nptr = backtrace(buffer, 100);
	backtrace_symbols_fd(buffer, nptr, fd);
}

void caller_pointer(void **ptr) {
	void *buffer[2];
	int nptr;
	nptr = backtrace(buffer, 2);
	if (nptr ==2) {
		*ptr = buffer[1];
	} else {
		*ptr = 0;
	}
}
