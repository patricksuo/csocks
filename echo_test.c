#include<stdio.h>


#include "task.h"
#include "malloc.h"
#include "net.h"
#include <stdint.h>

struct tcp_server_arg{
	char *host;
	int port;
};

void echo_task(void *arg) {
	char buff[512];
	char *buf_ptr;
	int fd = (int)(intptr_t)arg;
	int nread, nwrite;
	while ((nread = cs_read(fd, buff, 512)) > 0) {
		buf_ptr = buff;
		while (nread > 0) {
			nwrite = cs_write(fd, buf_ptr, nread);
			if (nwrite < 0) {
				goto end;
			} else {
				nread -= nwrite;
				buf_ptr += nwrite;
			}
		}
	}
end:
	cs_close_fd(fd);
}

void main_loop(void *arg) {
	struct tcp_server_arg *srv_arg = arg;
	int sfd, cfd;
	struct sockaddr_in caddr;
	socklen_t caddrlen;
	char caddrstr[INET_ADDRSTRLEN];
	printf("listen %s %d\n", srv_arg->host, srv_arg->port);
	sfd = cs_listen_tcp(srv_arg->host, srv_arg->port, 10);
	if (sfd < 0) {
		return; 
	}
	while (1) {
		cfd = cs_accept(sfd, (struct sockaddr *)&caddr, &caddrlen);
		if (cfd < 0) {
			break;
		}
		if (inet_ntop(AF_INET, (void *)&caddr, caddrstr, INET_ADDRSTRLEN)) {
			printf("accept %s\n", caddrstr);
		}
		task_new(echo_task, (void*)(intptr_t)cfd);
		break;
	}
	cs_close_fd(sfd);
}

int main(int argc, char **argv) {
	task_init(1024);
	struct tcp_server_arg *srv_arg;
	srv_arg = cs_malloc(sizeof(struct tcp_server_arg));
	srv_arg->host = argv[1];
	srv_arg->port = atoi(argv[2]);
	task_new(main_loop, srv_arg);
	task_schedule_loop();
	return 0;
}
