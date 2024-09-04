#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>

int main() {
    struct rlimit rlim;

    getrlimit(RLIMIT_NPROC, &rlim);
    printf("max user process : %lu / %lu\n", rlim.rlim_cur, rlim.rlim_max);

    getrlimit(RLIMIT_NOFILE, &rlim);
    printf("file size : %u, / %lu\n", (int)rlim.rlim_cur, rlim.rlim_max);

    getrlimit(RLIMIT_RSS, &rlim);
    printf("file memory size : %lu / %lu\n", rlim.rlim_cur, rlim.rlim_max);

    getrlimit(RLIMIT_CPU, &rlim);
    if(rlim.rlim_cur == RLIM_INFINITY) {
        printf("cpu time : UNLIMIT\n");
    }
    return (0);
}
