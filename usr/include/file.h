#ifndef _USR_FILE_H_
#define _USR_FILE_H_
#include <type.h>

int file_create(char *name);

int file_delete(char *name);

int file_write(char *name, addr_t addr, size_t size);

#endif