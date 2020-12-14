#include <syscall.h>
int main(void)
{
    SYSCALL();
    while(1);
}