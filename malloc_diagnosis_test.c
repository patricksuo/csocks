#include "malloc_diagnosis.h"
#include "malloc.h"

void *test_malloc_in_deep_stack(int i);
void test_free_in_deep_stack(int i, void *mem);

void *test_malloc_in_deep_stack(int i) {
	if (i>0) {
		i--;
		return test_malloc_in_deep_stack(i);
	}
	return cs_malloc(1024);
}

void test_free_in_deep_stack(int i, void *mem) {
	if (i>0) {
		i--;
		test_free_in_deep_stack(i, mem);
		return; 
	}
	cs_free(mem);
	return; 
}

int main(int argc, char **argv) {
	void *mem[10];
	int i,j;
	for (i=0; i<10; i++) {
		mem[i] = test_malloc_in_deep_stack(10);
	}
	for (i=0; i<10; i++) {
		test_free_in_deep_stack(10, mem[i]);
	}
	print_alloc_status(stderr);
	return 0;
}
