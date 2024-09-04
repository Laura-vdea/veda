#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void cleanup(void *arg)
{
    printf("%s called.\n", (char *)arg);
    free(arg);
    printf("cleanup : free called\n");
}

void * thread_function(void *arg)
{
	int length=strlen((char*)arg);
	char* ptr=(char*) malloc(length+strlen("_handler")+1);

    printf("thread_function start\n");
    strcpy(ptr, arg);
    strcat(ptr, "_handler");
    pthread_cleanup_push(cleanup, ptr);
    sleep(5);
    pthread_cleanup_pop(0);
    free(ptr);
    printf("thread_function : free called\n");
    return NULL;
}

int main(int argc, char* argv[])
{
    int ret;
    pthread_t tid;
    char* value;

    printf("main start\n");
    if(argc!=2) {
    	fprintf(stderr, "Usage : %s [thread | cleanup]\n", argv[0]);
    	exit(1);
    }
    value=argv[1];

    if((ret=pthread_create(&tid, NULL, thread_function, (void *)value))!=0) {
          perror("pthread_create1");
          exit(1);
    }
    if(strcmp(value, "cleanup")==0) {
    	sleep(2);
    	pthread_cancel(tid);
    }
    pthread_join(tid, NULL);
    printf("main end\n");
    return 0;
}
