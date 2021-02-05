#include <msg.h>
#include <syscall.h>

int msg_newmsg(msgd_t *msgd, void *addr, int size)
{
    return syscall(SYS_msg_newmsg, msgd, addr, size);
}

int msg_newbox(msgboxd_t *msgboxd)
{
    return syscall(SYS_msg_newbox, msgboxd);
}

int msg_send(msgboxd_t msgboxd, msgd_t msgd)
{
    return syscall(SYS_msg_send, msgboxd, msgd);
}

int msg_revcieve(msgboxd_t msgboxd, msgd_t *msgd)
{
    return syscall(SYS_msg_recieve, msgboxd, msgd);
}