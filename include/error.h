#ifndef _ERROR_H_
#define _ERROR_H_
enum ERRORS {
    E_OK,
    E_NOMEM,
    E_NOCACHE,
    E_MAX
};
const char *strerror(int err);
#endif