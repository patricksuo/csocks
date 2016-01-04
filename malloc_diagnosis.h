#include "config.h"
#include <stdio.h>

#ifndef MAX_RECORD_SLOT
#define MAX_RECORD_SLOT 1024
#endif
#ifndef MAX_CALL_CHAIN 
#define MAX_CALL_CHAIN 32
#endif


struct alloc_record_t {
	unsigned key;
	void *call_chain[MAX_CALL_CHAIN];
};

struct alloc_count_t {
	unsigned key;
	long count;
};

extern void leak_diagnosis_stat(int alloc);
extern void print_alloc_status(FILE *fp);

