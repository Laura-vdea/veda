#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

void sig_handler(int signo)
{
	printf("sig_handler : %x received signal %s\n",
			(int)pthread_self(), (signo==SIGUSR1)?"USR1":"USR2");
}

void *thread1_function(void *data)
{
    sigset_t newmask;
    struct sigaction act;
    int i, j;
    act.sa_handler = sig_handler;
    sigfillset(&act.sa_mask);
    act.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &act, NULL);

    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    pthread_sigmask(SIG_UNBLOCK, &newmask, NULL);
    for(i=0; i<30; i++) {
        printf("thread1 : tid = %x, i = %d\n", (int)pthread_self(),i);
        for(j=0; j<1999999999; j++);
        for(int k=0; k<1999999999; k++);
    }
    return NULL;
}

void *thread2_function(void *data)
{
    sigset_t newmask;
    struct sigaction act;
    int i , j;
    act.sa_handler = sig_handler;
    sigfillset(&act.sa_mask);
    act.sa_flags = SA_RESTART;
    sigaction(SIGUSR2, &act, NULL);

    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR2);
    pthread_sigmask(SIG_UNBLOCK, &newmask, NULL);

    for(i=0; i<30; i++) {
        printf("thread2 : tid = %x, i = %d\n", (int)pthread_self(),i);
        for(j=0; j<1999999999; j++);
        for(int k=0; k<1999999999; k++);
    }
    return NULL;
}

int main()
{
    int rc, i;
    sigset_t newmask;
    pthread_t thread_t[2];

    printf("PID : %d\n", getpid());
    sigfillset(&newmask);
    sigdelset(&newmask, SIGINT);
    pthread_sigmask(SIG_SETMASK, &newmask, NULL);

    if((rc=pthread_create(&thread_t[0], NULL, thread1_function, NULL))!=0) {
    	fprintf(stderr, "pthread_create : error code =%d\n", rc);
    	exit(1);
    }
    if((rc=pthread_create(&thread_t[1], NULL, thread2_function, NULL))!=0) {
       	fprintf(stderr, "pthread_create : error code =%d\n", rc);
       	exit(1);
    }
    pthread_join(thread_t[0], NULL);
    pthread_join(thread_t[1], NULL);
    return 0;
}
