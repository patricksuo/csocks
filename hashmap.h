#ifndef _CS_HASHMAP_H_
#define _CS_HASHMAP_H_
#include <stdint.h>

#define INT2PTR(x) (void*)(intptr_t)(x)

struct hashmap_t;
extern struct hashmap_t *hashmap_new(int cap);
extern void hashmap_upsert(struct hashmap_t *m, void *key, size_t keylen, void *new, void **oldp);
extern void *hashmap_get(struct hashmap_t *m, void *key, size_t keylen);
extern void hashmap_delete(struct hashmap_t *m, void *key, size_t keylen, void **oldp);

#endif /*_CS_HASHMAP_H_ */
