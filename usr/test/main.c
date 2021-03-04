#include <rwlock.h>
#include <proc.h>
struct rwlock_t lock;

void t1(void)
{
    while(1)
    {
        rwlock_read_lock(&lock);
        uint t = 100000000;
        while(t--);
        printf("t1 read get.\n");
        rwlock_read_unlock(&lock);
        printf("t1 read put.\n");
        rwlock_write_lock(&lock);
        t = 100000000;
        while(t--);
        printf("t1 write get.\n");
        rwlock_write_unlock(&lock);
        printf("t1 write put.\n");
    }
}

void t2(void)
{
    while(1)
    {
        rwlock_read_lock(&lock);
        uint t = 100000000;
        while(t--);
        printf("t2 read get.\n");
        rwlock_read_unlock(&lock);
        printf("t2 read put.\n");
        rwlock_write_lock(&lock);
        t = 100000000;
        while(t--);
        printf("t2 write get.\n");
        rwlock_write_unlock(&lock);
        printf("t2 write put.\n");
    }
}

void t3(void)
{
    while(1)
    {
        rwlock_read_lock(&lock);
        uint t = 100000000;
        while(t--);
        printf("t3 read get.\n");
        rwlock_read_unlock(&lock);
        printf("t3 read put.\n");
        rwlock_write_lock(&lock);
        t = 100000000;
        while(t--);
        printf("t3 write get.\n");
        rwlock_write_unlock(&lock);
        printf("t3 write put.\n");
    }
}

void t4(void)
{
    while(1)
    {
        rwlock_read_lock(&lock);
        uint t = 100000000;
        while(t--);
        printf("t4 read get.\n");
        rwlock_read_unlock(&lock);
        printf("t4 read put.\n");
        rwlock_write_lock(&lock);
        t = 100000000;
        while(t--);
        printf("t4 write get.\n");
        rwlock_write_unlock(&lock);
        printf("t4 write put.\n");
    }
}



int main(void)
{
    uint tid;
    rwlock_init(&lock);
    thread_create(&tid, t1, NULL, NULL);
    thread_create(&tid, t2, NULL, NULL);
    thread_create(&tid, t3, NULL, NULL);
    thread_create(&tid, t4, NULL, NULL);
    printf("thread created.\n");
}