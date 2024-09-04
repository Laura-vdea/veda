#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#define LOOP_MAX 10

int commonCounter = 0;
pthread_mutex_t mutexid;

void *inc_thread(void *);

int main(void)
{
    pthread_t tid1;
    pthread_t tid2;

    // mutex 생성
    pthread_mutex_init(&mutexid, NULL);

    if ((pthread_create(&tid1, NULL, inc_thread, "thread1")) ||
        (pthread_create(&tid2, NULL, inc_thread, "thread2"))) {
        perror("pthread_create");
        exit(errno);
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    // mutex 소멸
    pthread_mutex_destroy(&mutexid);

    return 0;
}

void *inc_thread(void *arg)
{
    int loopCount;
    int temp;
    char buffer[80];
    int i;
    int ret_value;

    for (loopCount = 0; loopCount < LOOP_MAX; loopCount++) {
        // mutex lock
        if ((ret_value=pthread_mutex_lock(&mutexid)) != 0) {
            perror("pthread_mutex_lock");
            pthread_exit(NULL);
        }

        sprintf(buffer, "<%s> Common counter : from %d to ", (char *)arg, commonCounter);
        write(1, buffer, strlen(buffer));

        temp = commonCounter;
        for (i = 0; i < 900000; i++); // for delay
        commonCounter = temp + 1;

        sprintf(buffer, "%d\n", commonCounter);
        write(1, buffer, strlen(buffer));

        // mutex unlock
        if ((ret_value=pthread_mutex_unlock(&mutexid)) != 0) {
            perror("pthread_mutex_unlock");
            pthread_exit(NULL);
        }

        for (i = 0; i < 500000; i++); // for delay
    }

    pthread_exit(NULL);
}
