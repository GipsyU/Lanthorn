#include <error.h>
const char *strerror(int err) {
    #define ERRCASE(_err) case _err: return # _err
    switch (err) {
        ERRCASE(E_OK);
        ERRCASE(E_MAX);
        ERRCASE(E_NOMEM);
        ERRCASE(E_NOPOS);
        ERRCASE(E_NOCACHE);
        ERRCASE(E_INVAL);
        default: return "E_UNKNOWN";
    }
    #undef ERRCASE
}