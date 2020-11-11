#include <error.h>
const char *strerror(int err) {
    #define ERRCASE(_err) case _err: return # _err
    switch (err) {
        ERRCASE(E_OK);
        ERRCASE(E_MAX);
        default: return "E_UNKNOWN";
    }
    #undef ERRCASE
}