#ifndef _USR_MSG_H_
#define _USR_MSG_H_
#include <type.h>

int msg_newmsg(uint *id, addr_t addr, size_t size);

int msg_newbox(uint *id);

int msg_send(uint box_id, uint msg_id);

int msg_recieve(uint box_id, uint *msg_id, uint is_block);

int msg_read(uint msg_id, addr_t cache, addr_t offset, size_t size);

#endif