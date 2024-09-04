#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    int pfd[2];

    pipe(pfd);      // pfd[0] : read pipe fd.
                    // pfd[1] : write pipe fd
    printf("%d, %d\n", pfd[0], pfd[1]);

    char buf[] = "hello parent";
    char buf1[1024] ;
    int n;

    switch(fork()) {
        case -1:
            perror("fork");
            exit(1);
            break;
        case 0:
            close(pfd[0]);
            dup2(pfd[1], 1);
            close(pfd[1]);

            // write(1, buf, strlen(buf));
            execlp("ps", "ps", NULL);
            perror("execlp");
            exit(1);
            break;
        default:
            close(pfd[1]);
            dup2(pfd[0], 0);

            // n=read(0, buf1, sizeof(buf1));
            // buf1[n] = '\0';
            execlp("wc", "wc", NULL);

            // printf("%s\n", buf1);
            wait(NULL);
            break;
    }
    return 0;
}
