#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>

int main()
{
    pid_t cpid;
    int status;

    cpid=fork();
    switch(cpid) {
        case -1:
            perror("fork");
            exit(1);
            break;
        case 0: // chlid process
            printf("pid : %d, ppid : %d\n", getpid(), getppid());
            sleep(2);
            exit(2);
            break;
        default:    // process process
            printf("cpid : %d, pid : %d\n", cpid, getpid());
            sleep(5);

            while(waitpid(cpid, &status, 0)!=cpid) {
                printf("parent process\n");
                sleep(1);
            }

            if(WIFEXITED(status)) {
                printf("exit code : %d\n", WEXITSTATUS(status));
            } else if(WIFSIGNALED(status)) {
                printf("killed by signal\n");
            }

            system("ps -l");
            break;
    }
}
