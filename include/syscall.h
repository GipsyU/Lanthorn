#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#define NR_SYSCALL 64

enum SYS_CALL
{
    SYS_none,
    SYS_fork,
    SYS_kill,
    SYS_malloc,
    SYS_mfree,
    SYS_poweroff,
    SYS_srv_call,
    SYS_srv_called,
    SYS_srv_reply,
    SYS_srv_register,
    SYS_srv_unregister,
    SYS_msg_send,
    SYS_msg_recieve,
    SYS_msg_read,
    SYS_msg_newbox,
    SYS_msg_newmsg,
    SYS_thread_block,
    SYS_thread_create,
    SYS_thread_exit,
    SYS_thread_tid,
    SYS_thread_wake,
    SYS_write
};

#ifndef __USER__

int syscall_register(uint id, addr_t func, uint nparam);

int syscall_unregister(uint id);

#endif

#endif