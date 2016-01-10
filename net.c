#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "net.h"
#include "poll.h"
#include "task.h"
#include "hashmap.h"
#include "log.h"
#include "malloc.h"

struct poll_note {
	void *reader;
	void *writer;
};

static int g_poll_fd;
static struct hashmap_t *g_fd_map;

int cs_net_init() {
	int pfd;
	pfd = cs_poll_create();
	if (pfd < 0) {
		SYS_ERR("cs_poll_create failed");
		return -1;
	} else {
		g_poll_fd = pfd;
		g_fd_map = hashmap_new(1024);
	}
	return  0;
}

int net_poll(int timeout) {
	struct cs_poll_event events[32];
	int nevent, i;
	struct poll_note *np;
	while(1) {
		nevent = cs_poll_wait(g_poll_fd, events, 32, timeout);
		if (nevent >= 0) {
			for (i=0; i<nevent; i++) {
				np = events[i].userdata;
				if (events[i].event|POLL_EV_READ && np->reader) {
					task_rejoin((struct cs_task_t *)np->reader);
				}
				if (events[i].event|POLL_EV_WRITE && np->writer) {
					task_rejoin((struct cs_task_t *)np->writer);
				}
			}
			return 0;
		} else {
			if (errno == EINTR) 
				continue;
			else 
				SYS_ERR("cs_poll_wait err");
				break;
		}
	}
	ERR("net_task_exit");
	return -1;
}


static int do_wait_fd(int fd, unsigned events) {
	int needsyscall;
	struct poll_note *note;
	void *current = task_current();
	note = hashmap_get(g_fd_map, &fd, sizeof(int));
	needsyscall = 0;
	/*
	if ((events | POLL_EV_READ) && note->reader && note->reader != current) {
		ERR("dup reader");
		return -1;
	}
	if ((events | POLL_EV_WRITE) && note->writer && note->writer != current) {
		ERR("dup writer");
		return -1;
	}
	*/
	if (events | POLL_EV_READ) {
		note->reader = current;
		needsyscall = (needsyscall || note->reader == NULL);
	}
	if (events | POLL_EV_WRITE) {
		note->writer = current;
		needsyscall = (needsyscall || note->writer == NULL);
	}
	if (note->reader) {
		events |= POLL_EV_READ;
	}
	if (note->writer) {
		events |= POLL_EV_WRITE;
	}

	if (needsyscall && (cs_poll_mod(g_poll_fd, fd, events, (void*)note) == -1)) {
		SYS_ERR("cs_poll_mod failed");
		return -1;
	}
	task_yield(0);
	return 0;
}

static int internal_release_fd(int fd) {
	struct poll_note *notep;
	void *task = (void*)task_current();
	notep = hashmap_get(g_fd_map, &fd, sizeof(int));
	if (!notep) {
		return -1; //not exist;
	}
	if (notep->reader == task) {
		notep->reader = NULL;
	}
	if (notep->writer == task) {
		notep->writer = NULL;
	}
	if (notep->reader == NULL && notep->writer == NULL) {
		hashmap_delete(g_fd_map, (void*)&fd, sizeof(int), (void*)&notep);
		cs_free(notep);
		cs_poll_del(g_poll_fd, fd);
		return 0; // a normal close
	}
	return 1; //remain reference;
}

static int internal_new_fd(int fd, int enable) {
	struct poll_note *note;
	void *oldp = NULL;
	note = cs_malloc(sizeof(struct poll_note));
	memset(note, 0, sizeof(struct poll_note));
	note->reader = enable ? (void*)task_current() : NULL;
	if (cs_poll_add(g_poll_fd, fd, POLL_EV_READ, (void *)note) == -1) {
		cs_free(note);
		return -1;
	}
	hashmap_upsert(g_fd_map, (void*)&fd, sizeof(int), (void*)note, &oldp);
	return 0;
}


ssize_t cs_read(int fd, void *buf, size_t count) {
	size_t nread = 0;
	while(1) {
		if (do_wait_fd(fd, POLL_EV_READ) != 0) {
			break;
		}
		nread = read(fd, buf, count);
		if (nread >= 0) {
			break;
		}
		/* err conditions 
		 * XXX Can read on non-blocking fd be interrupted ?
		 * */
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
			continue;
		}
		break;
	}
	return nread;
}

ssize_t cs_write(int fd, const void *buf, size_t count) {
	size_t nwrite = 0;
	while(1) {
		if (do_wait_fd(fd, POLL_EV_WRITE) != 0) {
			break;
		}
		nwrite = write(fd, buf, count);
		if (nwrite >= 0) {
			break;
		}
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
			continue;
		}
		SYS_ERR("write error");
		break;
	}
	return nwrite;
}

int cs_close_fd(int fd) {
	int status;
	char msg[32];
	status = internal_release_fd(fd);
	switch (status) {
		case -1:
			/* invalid or unmanaged fd */
			snprintf(msg, 32, "wild fd %d", fd);
			log(msg);
			break;
		case 0:
			/* nomarl close */
			close(fd);
			break;
		case 1:
			/* fd in use, don't close now */
			break;
	}
	return  status;
}

int cs_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	int cfd;
	while (1) {
		if (do_wait_fd(sockfd, POLL_EV_READ) == -1) {
			cs_poll_del(g_poll_fd, sockfd);
			return -1;
		}
		cfd = accept(sockfd, addr, addrlen);
		if (cfd > 0) {
			if (internal_new_fd(cfd, 0) == -1) {
				close(cfd);
				return -1;
			}
			return cfd;
		}
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			continue;
		}
		SYS_ERR("accept failed");
		return -1;

	}
	return 0;
}

static int do_nonblock(int fd) {
	int flags;
	flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0) {
		return -1;
	}
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static int do_connect(const char *host, int port, int proto) {
	int fd, status;
	char service[16];
	struct addrinfo *res, *rp;
	struct addrinfo hint;
	memset(&hint, 0, sizeof(struct addrinfo));
	if (proto == PROTO_TCP) {
		hint.ai_socktype = SOCK_STREAM;
	} else {
		hint.ai_socktype = SOCK_DGRAM;
	}
	status = getaddrinfo(host, service, &hint, &res);
	if (status != 0) {
		SYS_ERR("getaddrinfo failed");
		return -1;
	}
	for (rp=res,status=-1,fd=-1; rp!=NULL; rp=rp->ai_next) {
		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (fd == -1) {
			continue;
		}
		if (do_nonblock(fd) == -1) {
			close(fd);
			fd = -1;
			continue;
		}
		status = connect(fd, rp->ai_addr, rp->ai_addrlen);
		if (status == 0 ||  errno == EINPROGRESS) {
			break;
		}
		close(fd);
		fd = -1;
	}
	freeaddrinfo(res);
	if (fd == -1) return -1; // connect error

	if (internal_new_fd(fd, 1) == -1) {
		close(fd);
		return -1;
	}
	if (status == -1) { //connecting
		if (do_wait_fd(fd, POLL_EV_WRITE) == -1) {
			internal_release_fd(fd);
			cs_poll_del(g_poll_fd, fd);
			close(fd);
			return -1;
		}
	}
	return fd;
}

int cs_dial_tcp(const char *host, int port) {
	return do_connect(host, port, PROTO_TCP);
}

static int do_bind(const char *host, int port, int proto) {
	int fd, status;
	char service[16];
	struct addrinfo *res;
	struct addrinfo hint;
	if (host == NULL || host[0] == 0) {
		host = "0.0.0.0";
	}
	snprintf(service, 16, "%d", port);
	memset(&hint, 0, sizeof(struct addrinfo));
	if (proto == PROTO_TCP) {
		hint.ai_socktype = SOCK_STREAM;
	} else {
		hint.ai_socktype = SOCK_DGRAM;
	}
	status = getaddrinfo(host, service, &hint, &res);
	if (status != 0) {
		SYS_ERR(gai_strerror(status));
		return -1;
	}

	fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd < 0) {
		freeaddrinfo(res);
		return -1;
	}
	status = bind(fd, res->ai_addr, res->ai_addrlen);
	if (status == -1) {
		freeaddrinfo(res);
		close(fd);
		return -1;
	}
	return fd;
}

static int do_listen(char *host, int port, int proto, int backlog) {
	int fd;
	fd = do_bind(host, port, proto);
	if (fd < 0) {
		goto failed;
	}
	if (listen(fd, backlog) == -1) {
		goto failed_fd;
	}
	if (do_nonblock(fd) == -1) {
		goto failed_fd;
	}
	if (internal_new_fd(fd, 1) == -1) {
		goto failed_fd;
	}
	return fd;
failed_fd:
	close(fd);
failed:
	return -1;
}

int cs_listen_tcp(char *host, int port, int backlog) {
	return do_listen(host, port, PROTO_TCP, backlog);
}
