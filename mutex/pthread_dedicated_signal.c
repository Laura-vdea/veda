#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>

void* thread_function(void *x)
{
	int i;

	for (i = 0; i < 10; ++i) {
		printf("thread %d (loop #%d)\n", *((int *) x), i);
		sleep(3);
	}
	return (NULL);
}

void* signal_thread(void* arg)
{
	sigset_t sigset;
	int sig;

	sigemptyset(&sigset);
	sigaddset(&sigset, SIGUSR1);
	sigaddset(&sigset, SIGUSR2);
    while (1) {
	    sigwait(&sigset, &sig);
	    switch (sig) {
	    case SIGUSR1:
	    	 printf("---> caught signal SIGUSR1\n");
	    	 break;
	    case SIGUSR2:
	    	 printf("---> caught signal SIGUSR2\n");
	    	 break;
	    default:
	    	printf("unrecognized signal\n");
	    	break;
	    }
    }
}

int main(void)
{
	pthread_t sigtid, t1, t2;
	int sig, n1 = 1, n2 = 2, rc;
	sigset_t sigset;

	sigfillset(&sigset);
	sigdelset(&sigset, SIGINT);
	pthread_sigmask(SIG_SETMASK, &sigset, NULL);

	if((rc=pthread_create(&sigtid, NULL, signal_thread, NULL))!=0) {
		fprintf(stderr,"pthread_create(signal_thread) : rc = %d\n", rc);
		exit(1);
	}
	if((rc=pthread_create(&t1, NULL, thread_function, &n1))!=0) {
		fprintf(stderr,"pthread_create(signal_thread) : rc = %d\n", rc);
		exit(1);
	}
	if((rc=pthread_create(&t2, NULL, thread_function, &n2))!=0) {
		fprintf(stderr,"pthread_create(signal_thread) : rc = %d\n", rc);
		exit(1);
	}
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(sigtid, NULL);
	return 0;
}
