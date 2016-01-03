#ifndef _CS_UTILS_H_
#define _CS_UTILS_H_
extern void print_rusage(); 
extern void print_stats(); 
/**
 * print_stack - save call stack back trace to fd
 * @fd: if negative fd given, use STDERR_FILENO 
 */
extern void print_stack(int fd); 
extern void caller_pointer(void **ptr);
#endif
