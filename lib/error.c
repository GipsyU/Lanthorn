#include <error.h>
const char *strerror(int err) {
    #define ERRCASE(_err) case _err: return # _err
    switch (err) {
        ERRCASE(E_OK);
        ERRCASE(E_MAX);
        ERRCASE(E_FAULT);
        ERRCASE(E_NOTFOUND);
        ERRCASE(E_NOMEM);
        ERRCASE(E_NOSLOT);
        ERRCASE(E_NOCACHE);
        ERRCASE(E_INVAL);
        ERRCASE(E_AGAIN);
        ERRCASE(E_EXIST);
        ERRCASE(E_FAILE);
        ERRCASE(E_NOMSG);
        default: return "E_UNKNOWN";
    }
    #undef ERRCASE
}