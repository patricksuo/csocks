#ifndef _CS_LOG_H_
#define _CS_LOG_H_

#include<stdio.h>
#define log(msg) \
	fprintf(stderr, "[%s:%d] %s", __FILE__, __LINE__, msg)


//TODO print_trace
// http://stackoverflow.com/questions/3899870/print-call-stack-in-c-or-c

#endif /* _CS_LOG_H_ */
