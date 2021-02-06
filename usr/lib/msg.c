#include <msg.h>
#include <syscall.h>

int msg_newmsg(uint *id, addr_t addr, size_t size)
{
     return syscall(SYS_msg_newmsg, id, addr, size);
}

int msg_newbox(uint *id)
{
    return syscall(SYS_msg_newbox, id);
}

int msg_send(uint box_id, uint msg_id)
{
    return syscall(SYS_msg_send, box_id, msg_id);
}

int msg_recieve(uint box_id, uint *msg_id)
{
    return syscall(SYS_msg_recieve, box_id, msg_id);
}

int msg_read(uint msg_id, addr_t cache, size_t size)
{
    return syscall(SYS_msg_read, msg_id, cache, size);
}