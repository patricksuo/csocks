#ifndef _CS_BUFF_H_
#define _CS_BUFF_H_
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024 * 1024 * 16 
#define READ_BUFFER_SIZE 1024 * 2
#define MIN_BUFFER_CAP 512

struct cs_buffer_t;

extern struct cs_buffer_t *new_buffer(size_t cap);
extern void free_buffer(struct cs_buffer_t *);

//read from fd until EOF, growing the buffer as needed
extern size_t buffer_read_from(int fd, struct cs_buffer_t *buffer);
extern void grow_buffer(struct cs_buffer_t *buffer, size_t new_cap);
extern void reset_buffer(struct cs_buffer_t *buffer);
extern int write_buffer(struct cs_buffer_t *buffer, char *data, size_t size);

#endif /* define _CS_BUFF_H_ */
