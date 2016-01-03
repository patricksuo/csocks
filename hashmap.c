#include <string.h> /* memset */

#include "malloc.h"
//#include <stdio.h>


struct bucket_elem;
struct bucket_elem {
	int key;	
	void *data;
	struct bucket_elem *next;
};

struct hashmap_t {
	struct bucket_elem **bucket;
	int cap;
	int nelem;
	int ncollision;
};

static int hashmap_index(struct hashmap_t *m, int key) {
	return key%m->cap;
}	


struct hashmap_t *hashmap_new(int cap) {
	/* todo check cap */
	struct hashmap_t *m;
	m = cs_malloc(sizeof(struct hashmap_t));
	m->bucket = cs_malloc(cap * sizeof(struct bucket_elem));
	memset(m->bucket, 0, cap * sizeof(struct bucket_elem));
	m->cap = cap;
	m->nelem = 0;
	return m;
}

void hashmap_upsert(struct hashmap_t *m, int key, void *new, void **oldp) {
	struct bucket_elem *new_elem;
	struct bucket_elem *in_bucket;
	int idx = hashmap_index(m, key);
	in_bucket = m->bucket[idx];
	*oldp = NULL;

	if (in_bucket == NULL) {
		goto INSERT;
	}

	while (1) {
		if (in_bucket->key == key) { /* update old key's data */
			*oldp = in_bucket->data;
			in_bucket->data = new;
			//printf("update key %d data %p\n", key, new);
			return;
		}
		if (!in_bucket->next) { /* append one to the tail */
			break;
		}
		in_bucket = in_bucket->next;
	}

	m->ncollision++;

INSERT:
	new_elem = cs_malloc(sizeof(struct bucket_elem));
	new_elem->key = key;
	new_elem->data = new;
	//printf("insert key %d data %p\n", key, new);
	new_elem->next = NULL;
	if (in_bucket) {
		in_bucket->next = new_elem;
	} else {
		m->bucket[idx] = new_elem;
	}
	return;
}

void *hashmap_get(struct hashmap_t *m, int key) {
	struct bucket_elem *in_bucket;
	int idx = hashmap_index(m, key);
	in_bucket = m->bucket[idx];
	if (in_bucket == NULL) {
		return NULL;
	}

	while (1) {
		if (in_bucket->key == key) {
			return in_bucket->data;
		}
		if (in_bucket->next == NULL) {
			break;
		}
		in_bucket = in_bucket->next;
	}
	return NULL;
}

void hashmap_delete(struct hashmap_t *m, int key, void **oldp) {
	struct bucket_elem *in_bucket, *pre_bucket;
	int idx = hashmap_index(m, key);
	pre_bucket = NULL;
	in_bucket = m->bucket[idx];
	if (in_bucket == NULL) {
		*oldp = NULL;
		return;
	}
	while (1) {
		if (in_bucket->key == key) {
			goto need_free;
		}
		if (in_bucket->next) {
			pre_bucket = in_bucket;
			in_bucket = in_bucket->next;
		} else {
			*oldp = NULL;
			return;
		}
	}
need_free:
	//printf("delete key %d data %p\n", key, in_bucket->data);
	if (pre_bucket) {
		pre_bucket->next = in_bucket->next;
	}
	if (m->bucket[idx] == in_bucket) {
		m->bucket[idx] = in_bucket->next;
	}
	*oldp = in_bucket->data;
	cs_free(in_bucket);
	return;
}

/* TODO rehash 
 * static void hashmap_rehash(struct hashmap_t *m);
 * */
