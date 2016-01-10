#ifndef _CS_HASHMAP_H_
#define _CS_HASHMAP_H_
#include <stdint.h>

#define INT2PTR(x) (void*)(intptr_t)(x)

struct hashmap_t;
extern struct hashmap_t *hashmap_new(int cap);
extern void hashmap_upsert(struct hashmap_t *m, void *key, size_t keylen, void *new, void **oldp);
extern void *hashmap_get(struct hashmap_t *m, void *key, size_t keylen);
extern void hashmap_delete(struct hashmap_t *m, void *key, size_t keylen, void **oldp);
extern void hashmap_iter(struct hashmap_t *m, int(*fn)(void *, size_t));
extern unsigned oat_hash(void *key, size_t len); /*TODO move to separate file*/
extern int hashmap_elem_num(struct hashmap_t *m);

#endif /*_CS_HASHMAP_H_ */
