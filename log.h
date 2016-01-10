#ifndef _CS_LOG_H_
#define _CS_LOG_H_

#include<stdio.h>
#include <string.h>
#include <errno.h>
#define log(msg) \
	fprintf(stderr, "[%s:%d] %s", __FILE__, __LINE__, msg)

#define ERR(msg) fprintf(stderr, "[%s] [%s] at %s line %d\n", \
		msg,  __PRETTY_FUNCTION__, __FILE__, __LINE__)

#define SYS_ERR(msg) fprintf(stderr, "[%s] [%s] [%s] at %s line %d\n", \
		msg, strerror(errno), __PRETTY_FUNCTION__, __FILE__, __LINE__)


//TODO print_trace
// http://stackoverflow.com/questions/3899870/print-call-stack-in-c-or-c

#endif /* _CS_LOG_H_ */
