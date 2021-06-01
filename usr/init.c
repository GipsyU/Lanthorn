#include <error.h>
#include <proc.h>
#include <stdio.h>
#include <string.h>
#include <type.h>
int main(int argc, char *argv[])
{
    printf("Hello Lanthron.\n");

    printf("This is init proc.\n");

    struct proc_create_attr_t attr;

    memcpy(attr.name, "keyboard_service", sizeof("keyboard_service"));

    while (proc_create("/bin/keyboard.elf", &attr, NULL) != E_OK);

    memcpy(attr.name, "serial_service", sizeof("serial_service"));

    while (proc_create("/bin/serial.elf", &attr, NULL) != E_OK);

    memcpy(attr.name, "shell", sizeof("shell"));

    while (proc_create("/bin/shell.elf", &attr, NULL) != E_OK);
    
    printf("create shell success.\n");
    
    while (1);
}