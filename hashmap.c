#include <string.h> /* memset */

#include "malloc.h"
//#include <stdio.h>

struct bucket_elem;
struct bucket_elem {
	void *key;	
	size_t keylen;
	void *data;
	struct bucket_elem *next;
};

struct hashmap_t {
	struct bucket_elem **bucket;
	int cap;
	int nelem;
	int ncollision;
};

static void hashmap_rehash(struct hashmap_t *m);

static unsigned oat_hash(void *key, size_t len) {
	unsigned char *p = key;
	unsigned h = 0;
	int i;

	for (i = 0; i < len; i++) {
		h += p[i];
		h += (h << 10);
		h ^= (h >> 6);
	}

	h += (h << 3);
	h ^= (h >> 11);
	h += (h << 15);

	return h;
}

static unsigned hashmap_index(struct hashmap_t *m, void *key, size_t keylen) {
	unsigned idx = oat_hash(key, keylen);
	return idx%m->cap;
}	

/*
 * if a == b return 0 else return nonzero
 */
static int hashmap_key_equal(void *a, size_t alen, void *b, size_t blen) {
	if (alen != blen) return 0;
	return !memcmp(a, b, (size_t)alen);
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

void hashmap_upsert(struct hashmap_t *m, void *key, size_t keylen, void *new, void **oldp) {
	struct bucket_elem *new_elem;
	struct bucket_elem *in_bucket;
	unsigned idx = hashmap_index(m, key, keylen);
	in_bucket = m->bucket[idx];
	*oldp = NULL;

	if (in_bucket == NULL) {
		goto INSERT;
	}

	while (1) {
		if (hashmap_key_equal(in_bucket->key, in_bucket->keylen,
					key, keylen)) {  /* update old key's data */
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
	m->nelem++;
	new_elem = cs_malloc(sizeof(struct bucket_elem));
	new_elem->key = cs_malloc(keylen);
	memcpy(new_elem->key, key, keylen);
	new_elem->keylen = keylen;
	new_elem->data = new;
	//printf("insert key %d data %p\n", key, new);
	new_elem->next = NULL;
	if (in_bucket) {
		in_bucket->next = new_elem;
	} else {
		m->bucket[idx] = new_elem;
	}
	if (m->nelem > m->cap) hashmap_rehash(m);
	return;
}

void *hashmap_get(struct hashmap_t *m, void *key, size_t keylen) {
	struct bucket_elem *in_bucket;
	unsigned idx = hashmap_index(m, key, keylen);
	in_bucket = m->bucket[idx];
	if (in_bucket == NULL) {
		return NULL;
	}

	while (1) {
		if (hashmap_key_equal(in_bucket->key, in_bucket->keylen,
					key, keylen)) {  /* update old key's data */
			return in_bucket->data;
		}
		if (in_bucket->next == NULL) {
			break;
		}
		in_bucket = in_bucket->next;
	}
	return NULL;
}

void hashmap_delete(struct hashmap_t *m, void *key, size_t keylen, void **oldp) {
	struct bucket_elem *in_bucket, *pre_bucket;
	unsigned idx = hashmap_index(m, key, keylen);
	pre_bucket = NULL;
	in_bucket = m->bucket[idx];
	if (in_bucket == NULL) {
		*oldp = NULL;
		return;
	}
	while (1) {
		if (hashmap_key_equal(in_bucket->key, in_bucket->keylen,
					key, keylen)) {  /* update old key's data */
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
	m->nelem--;
	//printf("delete key %d data %p\n", key, in_bucket->data);
	if (pre_bucket) {
		pre_bucket->next = in_bucket->next;
	}
	if (m->bucket[idx] == in_bucket) {
		m->bucket[idx] = in_bucket->next;
	}
	*oldp = in_bucket->data;
	cs_free(in_bucket->key);
	cs_free(in_bucket);
	return;
}


static void hashmap_rehash(struct hashmap_t *m) {
	int cap, i;
	unsigned idx;
	struct bucket_elem **bucket; 
	struct bucket_elem *old_bk_elm, *new_bk_elm;

	cap = m->cap;
	m->cap *= 2; /* hashma_index deps on m->cap, so mult this value early */
       	bucket = cs_malloc(m->cap * sizeof(struct bucket_elem));
	memset(bucket, 0, m->cap * sizeof(struct bucket_elem));

	for (i=0; i<cap; i++) {
		while(m->bucket[i]) {
			old_bk_elm = m->bucket[i];
			m->bucket[i] = old_bk_elm->next;
			old_bk_elm->next = NULL;

			idx = hashmap_index(m, old_bk_elm->key, old_bk_elm->keylen);
			new_bk_elm = bucket[idx];
			if (!new_bk_elm) {
				bucket[idx] = old_bk_elm;
			} else {
				while(new_bk_elm->next) {
					new_bk_elm = new_bk_elm->next;
				}
				new_bk_elm->next = old_bk_elm;
			}
		}
	}
	cs_free(m->bucket);
	m->bucket = bucket;
	m->ncollision = 0;
}
