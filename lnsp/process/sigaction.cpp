#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void int_handler(int signo) {
    printf("Caught SIGINT\n");
}

void killprocess(int signo) {
    printf("Caught SIGTERM or SIGUSR1\n");
    exit(1);  // 임의로 프로세스를 종료
}

void quitprocess(int signo) {
    printf("Caught SIGQUIT\n");
    exit(1);
}

int main() {
    struct sigaction sigact, oldsigact;

    // SIGINT 처리
    sigact.sa_handler = int_handler;
    sigfillset(&sigact.sa_mask);
    sigact.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sigact, &oldsigact);

    // SIGQUIT 처리
    sigact.sa_handler = quitprocess;
    sigaction(SIGQUIT, &sigact, &oldsigact);

    // SIGTERM 또는 SIGUSR1 처리
    sigact.sa_handler = killprocess;
    sigaction(SIGTERM, &sigact, &oldsigact);

    for (;;) {
        printf("signal test\n");
        sleep(1);
    }
    return 0;
}
