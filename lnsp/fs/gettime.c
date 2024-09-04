#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

int main()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    printf("tv_sec : %d, tv_userc : %ld\n", (int) tv.tv_sec, tv.tv_usec);

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    printf("tv_sec : %d, tv_nes : %ld\n", (int) ts.tv_sec, ts.tv_nsec);

    time_t rawtime;
    time(&rawtime);
    printf("current time : %u\n", (unsigned)rawtime);

    struct timeval mytime;
    gettimeofday(&mytime, NULL);
    printf("gettimeofday : %ld/%ld\n", mytime.tv_sec, mytime.tv_usec);

    printf("current time : %s", ctime(&rawtime));

    putenv("TZ=PST3PDT");
    tzset();
    struct tm *tm;
    tm = localtime(&rawtime);
    printf("asctime : %s", asctime(tm));

    char buf[BUFSIZ];
    strftime(buf, sizeof(buf), "%a %b %e %H:%M:%S %Y", tm);
    printf("strftime : %s\n", buf);

    return (0);
}
