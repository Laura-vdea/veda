#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main()
{
    sigset_t sigmask;
    sigfillset(&sigmask);
    sigprocmask(SIG_BLOCK,&sigmask,NULL);

    for(int i=0; i<10; i++) {
        printf("sigpromask\n");
        sleep(1);
    }

    sigprocmask(SIG_UNBLOCK, &sigmask, NULL);

    return 0;
}
