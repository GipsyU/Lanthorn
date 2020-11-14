#ifndef _ERROR_H_
#define _ERROR_H_
/*
 * WARNING: must modify the error.c in the same time
 */
enum ERRORS {
    E_OK,
    E_NOMEM,
    E_NOPOS,
    E_NOCACHE,
    E_INVAL,
    E_MAX
};
const char *strerror(int err);
#endif