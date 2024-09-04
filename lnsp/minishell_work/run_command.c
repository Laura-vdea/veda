#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

int is_background(char** arglist);
int redirect_out(char** arglist);
int run_command(char** arglist);

int run_command(char** arglist) {
    pid_t cpid;
    int back_flag = 0;

    if (is_background(arglist))
        back_flag = 1;

    for (int i = 0; arglist[i] != NULL; i++) {
        if (strcmp(arglist[i], "|") == 0) {
            arglist[i] = NULL; // 현재 명령어 종료
            int pipefd[2];
            if (pipe(pipefd) == -1) {
                perror("pipe");
                return -1;
            }

            pid_t pid2 = fork();
            if (pid2 == -1) {
                perror("fork");
                return -1;
            } else if (pid2 == 0) { // 첫 번째 명령어 실행
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
                execvp(arglist[0], arglist);
                perror("execvp");
                exit(1);
            } else { // 두 번째 명령어 실행
                close(pipefd[1]);
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[0]);
                execvp(arglist[i + 1], &arglist[i + 1]);
                perror("execvp");
                exit(1);
            }
        } else if (strcmp(arglist[i], ">") == 0) {
            if (redirect_out(arglist) == -1) {
                return -1;
            }
        }
    }

    // 기본 명령어 실행
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        return -1;
    }

    if (cpid == 0) { // 자식 프로세스
        execvp(arglist[0], arglist);
        perror("execvp");
        exit(1);
    } else { // 부모 프로세스
        if (back_flag == 0) { // 백그라운드 실행이 아닌 경우
            int status;
            waitpid(cpid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                return 0;
            } else {
                return -1;
            }
        }
        return 0;
    }
}

int is_background(char** arglist) {
    int i;
    for(i = 0; arglist[i] != NULL; i++);
    if(strcmp(arglist[i-1], "&") == 0) {
        arglist[i-1] = NULL; // "&" 삭제
        return 1;
    }
    return 0;
}

int redirect_out(char** arglist) {
    int i, fd;
    for(i = 0; arglist[i] != NULL; i++) {
        if(strcmp(arglist[i], ">") == 0) {
            fd = open(arglist[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if(fd == -1) {
                perror("open");
                return -1;
            }
            if (dup2(fd, STDOUT_FILENO) == -1) {
                perror("dup2");
                close(fd);
                return -1;
            }
            close(fd);
            arglist[i] = NULL;
            return 0;
        }
    }
    return 0;
}
