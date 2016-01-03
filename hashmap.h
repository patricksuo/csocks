#ifndef _CS_HASHMAP_H_
#define _CS_HASHMAP_H_

struct hashmap_t;
extern struct hashmap_t *hashmap_new(int cap);
extern void hashmap_upsert(struct hashmap_t *m, int key, void *new, void **oldp);
extern void *hashmap_get(struct hashmap_t *m, int key);
extern void hashmap_delete(struct hashmap_t *m, int key, void **oldp);

#endif /*_CS_HASHMAP_H_ */
