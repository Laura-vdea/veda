#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void int_handler(int signo) {
    printf("int_handler\n");
    count = count+1;
}

int main() {
    // signal(SIGINT, SIG_TGN));
    signal(SIGINT, int_handler);
    for(;;) {
        printf("siganl test\n");
        sleep(1);
    }
    return (0);
}
