#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int status;

    if(fork() == 0) { // child process
        char *arglist[] = { "ls", "ls", "-l", NULL };

        printf("child start\n");
        // execl("/bin/ls", "ls", "-l", NULL);
        // execlp("ls", "ls", "-l", NULL);
        execv(arglist[0], &arglist[1]);
        printf("child end\n");
        exit(0);
    } else {
        wait(&status);
        if(WIFEXITED(status)) {
            printf("exit code : %d\n", WEXITSTATUS(status));
        } else if(WIFSIGNALED(status)) {
            printf("killed by signal\n");
        }
    }
    return (0);
}
