#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>

#define LOOP_MAX 10

int commonCounter = 0;
sem_t semid;

void *inc_thread(void *arg);

int main(void)
{
    pthread_t tid1, tid2;

    // 세마포어 초기화
    if (sem_init(&semid, 0, 1) != 0) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    if ((pthread_create(&tid1, NULL, inc_thread, "thread1")) ||
        (pthread_create(&tid2, NULL, inc_thread, "thread2"))) {
        perror("pthread_create");
        exit(errno);
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    sem_destroy(&semid); // 세마포어 삭제
    return 0;
}

void *inc_thread(void *arg)
{
    int loopCount;
    int temp;
    char buffer[80];
    int i;

    for (loopCount = 0; loopCount < LOOP_MAX; loopCount++) {
        // 세마포어 잠금
        if (sem_wait(&semid) == -1) {
            perror("sem_wait");
            pthread_exit(NULL);
        }

        sprintf(buffer, "<%s> Common counter : from %d to ", (char *)arg, commonCounter);
        write(1, buffer, strlen(buffer));

        temp = commonCounter;
        for (i = 0; i < 900000; i++); // for delay
        commonCounter = temp + 1;
        sprintf(buffer, "%d\n", commonCounter);
        write(1, buffer, strlen(buffer));

        // 세마포어 잠금 해제
        if (sem_post(&semid) == -1) {
            perror("sem_post");
            pthread_exit(NULL);
        }

        // for (i = 0; i < 500000; i++); // for delay
    }

    pthread_exit(NULL);
}
