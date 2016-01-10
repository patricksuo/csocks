#include "poll.h"
#include "log.h"

#include <sys/epoll.h>
#include <stdlib.h>

static int g_n_polling_fd;

int cs_poll_create() {
	return epoll_create1(0);
}

int cs_poll_add(int pfd, int sfd, unsigned ev, void *usrdata) {
	struct epoll_event event;
	int status;
	if (!IS_EV_VALID(ev)) {
		return -1;
	}
	if (ev&POLL_EV_READ) 
		event.events |= EPOLLIN;
	if (ev&POLL_EV_WRITE) 
		event.events |= EPOLLOUT;
	if (ev&POLL_EV_ERR)
		event.events |= EPOLLERR;
	event.data.ptr = usrdata;
	status = epoll_ctl(pfd, EPOLL_CTL_ADD, sfd, &event);
	if (status == 0) g_n_polling_fd++;
	return  status;
}

int cs_poll_mod(int pfd, int sfd, unsigned ev, void *usrdata) {
	struct epoll_event event;
	if (!IS_EV_VALID(ev)) {
		return -1;
	}
	if (ev&POLL_EV_READ) 
		event.events |= EPOLLIN;
	if (ev&POLL_EV_WRITE) 
		event.events |= EPOLLOUT;
	if (ev&POLL_EV_ERR)
		event.events |= EPOLLERR;
	event.data.ptr = usrdata;
	return epoll_ctl(pfd, EPOLL_CTL_MOD, sfd, &event);
}

int cs_poll_del(int pfd, int sfd) {
	struct epoll_event event;
	int status;
	status = epoll_ctl(pfd, EPOLL_CTL_DEL, sfd, &event);
	if (status == 0) g_n_polling_fd--;
	return status;
}

int cs_poll_wait(int pfd, struct cs_poll_event *events, int maxevents, int timeout_ms) {
	int nevent, i;
	struct epoll_event ep_events[maxevents];
	printf("poll_wait %d %d\n", g_n_polling_fd, timeout_ms);
	if (g_n_polling_fd == 0) return 0;
	nevent = epoll_wait(pfd, ep_events, maxevents, timeout_ms);
	if (nevent > 0) {
		for (i=0; i<nevent; i++) {
			events[i].userdata = ep_events[i].data.ptr;
			events[i].event = 0;
			if (ep_events[i].events&EPOLLIN) {
				events[i].event |= POLL_EV_READ;
			}
			if (ep_events[i].events&EPOLLOUT) {
				events[i].event |= POLL_EV_WRITE;
			}
			if (ep_events[i].events&EPOLLERR) {
				events[i].event |= POLL_EV_ERR;
			}
		}
	}
	return nevent;
}
