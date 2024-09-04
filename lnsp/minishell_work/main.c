#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "minishell.h"

#define MAXLINE 255

void int_handler(int signo) {
    (void)signo;  // 사용하지 않는 매개변수를 무시합니다.
    while(waitpid(0, NULL, WNOHANG) == 0); // 종료될 때까지 기다림
}

int main(void) {
    char line[MAXLINE];
    char command[MAXLINE * 10];  // 여러 줄 입력을 처리하기 위한 큰 버퍼
    char **arglist;

    struct sigaction sigact;
    sigact.sa_handler = int_handler;
    sigfillset(&sigact.sa_mask);
    sigact.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sigact, NULL);

    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    while (1) {
        // 프롬프트 출력
        fputs("minishell> ", stdout);
        fflush(stdout);

        // 명령어 입력 초기화
        command[0] = '\0';

        // 여러 줄 입력 처리 루프
        while (fgets(line, MAXLINE, stdin)) {
            // 엔터만 입력된 경우 루프를 계속 돕니다.
            // if (strcmp(line, "\n") == 0) {
            //    continue;
            // }

            // 종료 명령어 처리
            if (!strncmp(line, "exit", 4) || !strncmp(line, "quit", 4)) {
                return 0;
            }

            // "next >" 가 입력되면 다음 줄로 계속 입력 받음
            if (strstr(line, "next >") != NULL) {
                strcat(command, line);  // 입력을 command에 추가
                fputs("next > ", stdout);
                fflush(stdout);
            } else {
                strcat(command, line);  // 마지막 입력을 추가하고 종료
                break;
            }
        }

        // 명령어 파싱 및 실행
        arglist = command_parse(command);
        if (arglist == NULL || arglist[0] == NULL) {
            fprintf(stderr, "명령어 파싱 실패\n");
            continue;  // 파싱 실패 시 다시 프롬프트로 돌아갑니다.
        }

        if (run_command(arglist) == -1) {
            fprintf(stderr, "잘못된 명령어입니다: %s", command);
        }

        command_freelist(arglist);
    }

    return 0;
}
