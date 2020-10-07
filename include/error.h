#ifndef _ERROR_H_
#define _ERROR_H_
enum ERRORS {
    E_OK,
    E_MAX
};
const char *strerror(int err) {
    #define ERRCASE(_err) case _err: return # _err
    switch (err) {
        ERRCASE(E_OK);
        ERRCASE(E_MAX);
        default: return "E_UNKNOWN";
    }
    #undef ERRCASE
}
#endif