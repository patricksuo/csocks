#include <execinfo.h>
#include <string.h>

#include "malloc_diagnosis.h"
#include "hashmap.h"

#if DEBUG_ALLOC
static int nalloc_slot;
static int nfree_slot;


static struct alloc_record_t alloc_record[MAX_RECORD_SLOT];
static struct alloc_count_t alloc_count[MAX_RECORD_SLOT];
static struct alloc_record_t free_record[MAX_RECORD_SLOT];
static struct alloc_count_t free_count[MAX_RECORD_SLOT];

void leak_diagnosis_stat(int alloc) {
	if (alloc == 0) return;
	struct alloc_record_t *record;
	struct alloc_count_t *count;
	int idx=0;
	unsigned key;
	int ncallchain=0;
	void *callchain[MAX_RECORD_SLOT];
	ncallchain = backtrace(callchain, MAX_CALL_CHAIN);
	key = oat_hash(callchain, ncallchain * sizeof(void*));
	if (alloc > 0) {
		record = alloc_record;
		count = alloc_count;
	} else {
		record = free_record;
		count = free_count;
	}

	for (idx=0; idx< MAX_RECORD_SLOT; idx++) {
		if (record[idx].key==0 ||
				key == record[idx].key) {
			break;
		}
	}
	if (key == record[idx].key) {
		count[idx].count++;
	} else if (record[idx].key==0) {
		count[idx].count = 1;
		count[idx].key = key;
		record[idx].key = key;
		memcpy(record[idx].call_chain, callchain, ncallchain*sizeof(void*));
	}
}

void print_alloc_status(FILE *fp) {
	int i, j;
	void *funcptr;
	for (i=0; i<MAX_RECORD_SLOT; i++){
		if (!alloc_record[i].key) break;
		fprintf(fp, "alloc: ");
		for (j=0; j<MAX_CALL_CHAIN; j++) {
			funcptr = alloc_record[i].call_chain[j];
			if (!funcptr) break;
			fprintf(fp, "%p->", funcptr);
		}
		fprintf(fp, "null : %ld\n", alloc_count[i].count);
	}
	for (i=0; i<MAX_RECORD_SLOT; i++){
		if (!free_record[i].key) break;
		fprintf(fp, "free: ");
		for (j=0; j<MAX_CALL_CHAIN; j++) {
			funcptr = free_record[i].call_chain[j];
			if (!funcptr) break;
			fprintf(fp, "%p->", funcptr);
		}
		fprintf(fp, "null : %ld\n", free_count[i].count);
	}
}

#else
void leak_diagnosis_stat(int alloc) {}
void print_alloc_status(FILE *fp) {}
#endif

