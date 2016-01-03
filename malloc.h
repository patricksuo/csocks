#ifndef _CS_MALLOC_H_
#define _CS_MALLOC_H_

#include<stdlib.h>

extern void *cs_malloc(size_t size);
extern void cs_free(void *ptr);
extern void *cs_calloc(size_t nmemb, size_t size);
extern void *cs_realloc(void *ptr, size_t size);

#endif /* _CS_MALLOC_H_ */
