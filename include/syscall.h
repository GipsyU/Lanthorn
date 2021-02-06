#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#define NR_SYSCALL 64

#define SYS_none 0
#define SYS_read 1
#define SYS_exit 2
#define SYS_exec 3
#define SYS_fork 4
#define SYS_open 5
#define SYS_close 6
#define SYS_write 7
#define SYS_malloc 8
#define SYS_mfree 9
#define SYS_kill 10
#define SYS_poweroff 11
#define SYS_servercall 12
#define SYS_msg_send 13
#define SYS_msg_recieve 14
#define SYS_msg_read 15
#define SYS_msg_newbox 16
#define SYS_msg_newmsg 17

#ifndef __USER__

int syscall_register(uint id, addr_t func, uint nparam);

int syscall_unregister(uint id);

#endif

#endif