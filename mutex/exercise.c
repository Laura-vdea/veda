#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define NUMTHREADS 3
pthread_mutex_t mutexid =
PTHREAD_MUTEX_INITIALIZER;
int count=0;

void cleanup(void *arg) {
    printf("%s called.\n", (char*)arg);
    free(arg);
    printf("cleanup : free called\n");
}


void *thread_function(void *arg) {
    int i;
    int length=strlen((char*)arg);
    char* ptr =(char*)malloc(length+strlen("_handler")+1);

    printf("thread_Function start\n");
    strcpy(ptr, arg);
    strcat(ptr, "_handler");
    pthread_cleanup_push(cleanup, ptr);


    for(i=0; i<5; i++) {
        pthread_mutex_lock(&mutexid);
        count++;
        printf("count = %d\n", count);
        sleep(1);
        pthread_mutex_unlock(&mutexid);
    }

    pthread_cleanup_pop(0);
    free(ptr);
    printf("thread_function : free called\n");
    return NULL;
}

int main(int argc, char* argv[]) {
    pthread_t threads[NUMTHREADS];
    int i;
    int ret;
    pthread_t tid;
    char* value;

    printf("main start\n");
    if(argc!=2) {
        fprintf(stderr, "Usage : %s [thread | cleanup]\n", argv[0]);
        exit(1);
    }

    value=argv[1];
    if((ret=pthread_create(&tid, NULL, thread_function, (void*)value)) != 0) {
        perror("pthread_create 1");
        exit(1);
    }

    for(i=0; i<NUMTHREADS; i++) {
        pthread_create(&threads[i], NULL, thread_function, NULL);
    }

    for(i=0; i<NUMTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_join(tid, NULL);
    printf("main end\n");

    return 0;

}
