#ifndef _USR_ERROR_H_
#define _USR_ERROR_H_
/*
 * WARNING: must modify the error.c in the same time
 */
enum ERRORS {
    E_OK,
    E_NOMEM,
    E_FAULT,
    E_NOSLOT,
    E_NOCACHE,
    E_INVAL,
    E_AGAIN,
    E_NOTFOUND,
    E_EXIST,
    E_FAILE,
    E_NOMSG,
    E_BLOCK,
    E_TOOLONG,
    E_MAX
};
const char *strerror(int err);
#endif