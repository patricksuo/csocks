#ifndef _CS_NET_H_
#define _CS_NET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PROTO_TCP 1
#define PROTO_UDP 2

extern int cs_net_init();
extern int net_poll(int timeout);
extern ssize_t cs_read(int fd, void *buf, size_t count);
extern ssize_t cs_write(int fd, const void *buf, size_t count);
extern int cs_close_fd(int fd);
extern int cs_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
extern int cs_dial_tcp(const char *host, int port);
extern int cs_listen_tcp(char *host, int port, int backlog);

#endif
