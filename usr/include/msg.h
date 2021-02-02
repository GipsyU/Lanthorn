#ifndef _USR_MSG_H_
#define _USR_MSG_H_
#include <type.h>

typedef addr_t msgd_t;

typedef addr_t msgboxd_t;

int msg_newmsg(msgd_t *msgd, addr_t addr, size_t size);

int msg_newbox(msgboxd_t *msgboxd);

int msg_send(msgboxd_t msgboxd, msgd_t msgd);

int msg_revcieve(msgboxd_t msgboxd, msgd_t *msgd);

#endif