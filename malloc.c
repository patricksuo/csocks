#include <stdlib.h>

#include "malloc.h"
#include "log.h"
#include "config.h"

void *cs_malloc(size_t size) {
	void *ptr;
	ptr = malloc(size);
	if (ptr == NULL) {
		log("malloc failed");
		exit(EXIT_FAILURE);
	}
	return ptr;
}

void cs_free(void *ptr) {
	free(ptr);
}

void *cs_calloc(size_t nmem, size_t size) {
	void *ptr;
	ptr = calloc(nmem, size);
	if (ptr == NULL) {
		log("cmalloc failed");
		exit(EXIT_FAILURE);
	}
	return ptr;
}

void *cs_realloc(void *ptr, size_t size) {
	void *new_ptr;
	new_ptr = realloc(ptr, size);
	if (new_ptr == NULL) {
		log("realloc failed");
		exit(EXIT_FAILURE);
	}
	return new_ptr;
}
