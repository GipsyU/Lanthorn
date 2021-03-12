#include <srv.h>
#include <log.h>
#include <error.h>
char s[] = "fssrv create enable.\n";
int main(void)
{
    struct srv_replyee_t replyee;

    uint type = 0;

    int err = srv_call("fssrv/create", &replyee, "/lib/2.txt", sizeof("/lib/2.txt"), &type, sizeof(uint), s, sizeof(s));

    srv_call("fssrv/read", &replyee, "/lib/2.txt", sizeof("/lib/2.txt"));
    
    printf("%s", replyee.cache);

    err = srv_call("fssrv/delete", &replyee, "/lib/2.txt", sizeof("/lib/2.txt"));

    printf("%s %s\n", strerror(err), strerror(replyee.err));

    srv_call("fssrv/read", &replyee, "/lib/2.txt", sizeof("/lib/2.txt"));
    
    printf("%s %s\n", strerror(err), strerror(replyee.err));
    
    printf("%s", replyee.cache);

    return 0;
}