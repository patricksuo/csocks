#ifndef _CS_POLL_H_
#define _CS_POLL_H_

#define POLL_EV_READ 	((unsigned)(1<<0))
#define POLL_EV_WRITE 	((unsigned)(1<<1))
#define POLL_EV_ERR 	((unsigned)(1<<2))
#define POLL_EV_ALL 	(POLL_EV_READ|POLL_EV_WRITE|POLL_EV_ERR)
#define IS_EV_VALID(e) (((unsigned)(e)) & POLL_EV_ALL)

struct cs_poll_event {
	void *userdata;
	unsigned event;
};

extern int cs_poll_create();
extern int cs_poll_add(int pfd, int sfd, unsigned ev, void *usrdata);
extern int cs_poll_mod(int pfd, int sfd, unsigned ev, void *usrdata);
extern int cs_poll_del(int pfd, int sfd);
extern int cs_poll_wait(int pfd, struct cs_poll_event *events, int maxevents, int timeout);
#endif
