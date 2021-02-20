#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#define NR_SYSCALL 64

#define SYS_none 0
#define SYS_fork 1
#define SYS_kill 2
#define SYS_malloc 3
#define SYS_mfree 4
#define SYS_poweroff 5
#define SYS_servercall 6
#define SYS_msg_send 7
#define SYS_msg_recieve 8
#define SYS_msg_read 9
#define SYS_msg_newbox 10
#define SYS_msg_newmsg 11
#define SYS_thread_exit 12
#define SYS_thread_new 13
#define SYS_write 14

#ifndef __USER__

int syscall_register(uint id, addr_t func, uint nparam);

int syscall_unregister(uint id);

#endif

#endif