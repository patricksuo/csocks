#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "buff.h"
#include "log.h"
#include "malloc.h"

struct cs_buffer_t {
	char *buff;
	size_t cap;
	size_t off;
};

struct cs_buffer_t *new_buffer(size_t cap) {
	struct cs_buffer_t *buffer ;

	buffer = cs_malloc(sizeof (struct cs_buffer_t));
	memset(buffer, sizeof (struct cs_buffer_t), 0);

	if (cap > 0) {
		buffer->buff = cs_malloc(cap);
		buffer->cap = cap;
	} 

	return buffer;
}

void free_buffer(struct cs_buffer_t * buffer) {
	assert(buffer);
	if (buffer->buff) {
		cs_free(buffer->buff);
	}
	cs_free(buffer);
}

void grow_buffer(struct cs_buffer_t *buffer, size_t new_cap) {
	if (buffer->cap >= new_cap) {
		return;
	}
	buffer->buff = cs_realloc(buffer->buff, buffer->cap);
	buffer->cap = new_cap;
}

void reset_buffer(struct cs_buffer_t *buffer) {
	buffer->off = 0;
}

int write_buffer(struct cs_buffer_t *buffer, char *data, size_t size) {
	if (size > MAX_BUFFER_SIZE) {
		log("buffer too large");
		return -1;
	}
	if (buffer->cap - buffer->off < size) {
		size_t new_cap;
		new_cap = size - buffer->cap + buffer->off;
		if (new_cap > MAX_BUFFER_SIZE || new_cap < buffer->cap) {
			log("buffer too large");
			return -1;
		}
		grow_buffer(buffer, new_cap);
	}
	memcpy(buffer->buff+buffer->off, data, size);
	buffer->off += size;
	return 0;
}

/*
size_t buffer_read_from(int fd, struct cs_buffer_t *buffer) {
	size_t readn = 0;
	if ( buffer->cap - buffer->off < MIN_BUFFER_CAP) {
		size_t new_cap = buffer->cap * 2 + MIN_BUFFER_CAP;
		new_cap = new_cap < MAX_BUFFER_SIZE ? new_cap : MAX_BUFFER_SIZE;
		grow_buffer(buffer, new_cap);
	}
	if (buffer->cap == buffer->off) {
		log("buffer full");
		return readn;
	}
	return readn;
}

size_t buffer_write_to(int fd, struct cs_buffer_t *buffer) {
	return 0;
}
*/
