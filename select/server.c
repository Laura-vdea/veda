#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/select.h>

#define DEFAULT_TCP_PORT 5100

int create_network_tcp_process(int num_tcp_proc, ...);

int main() {
    printf("VEDA CHAT PROGRAM\n");
    usleep(1000);
    create_network_tcp_process(1, "127.0.0.1", DEFAULT_TCP_PORT);
    return 0;
}

int create_network_tcp_process(int num_tcp_proc, ...) {
    va_list args;
    va_start(args, num_tcp_proc);

    for (int i = 0; i < num_tcp_proc; i++) {
        int ssock, csock;
        socklen_t clen;
        struct sockaddr_in servaddr, cliaddr;
        char buffer[1024];
        fd_set readfds;
        int max_sd;

        const char *ip_address = va_arg(args, const char*);
        int port = va_arg(args, int);

        // 소켓 생성
        if ((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket()");
            return -1;
        }

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        //servaddr.sin_addr.s_addr = inet_addr(ip_address);
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(port);

        // 소켓 바인딩
        if (bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
            perror("bind()");
            return -1;
        }

        // 클라이언트의 연결 요청 대기
        if (listen(ssock, 8) < 0) {
            perror("listen()");
            return -1;
        }

        printf("서버가 클라이언트의 연결을 기다립니다...\n");

        clen = sizeof(cliaddr);
        csock = accept(ssock, (struct sockaddr *)&cliaddr, &clen);
        if (csock < 0) {
            perror("accept()");
            return -1;
        }

        printf("클라이언트가 연결되었습니다.\n");

        while (1) {
            FD_ZERO(&readfds);
            FD_SET(0, &readfds);      // 표준 입력 감시
            FD_SET(csock, &readfds);  // 클라이언트 소켓 감시
            max_sd = csock;

            int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

            if (activity < 0) {
                perror("select()");
                break;
            }

            // 사용자 입력 감지
            if (FD_ISSET(0, &readfds)) {
                fgets(buffer, 1024, stdin);
                if (strncmp(buffer, "exit", 4) == 0) {
                    printf("서버 종료\n");
                    break;
                }

                send(csock, buffer, strlen(buffer), 0);
                // printf("서버: %s", buffer);
            }

            // 클라이언트로부터 메시지 수신
            if (FD_ISSET(csock, &readfds)) {
                int valread = read(csock, buffer, 1024);
                if (valread <= 0) {
                    printf("클라이언트 연결이 종료되었습니다.\n");
                    break;
                }
                buffer[valread] = '\0';
                printf("클라이언트: %s", buffer);
            }
        }

        close(csock);  // 클라이언트 소켓 닫기

        va_end(args);

        close(ssock);  // 서버 소켓 닫기
    }
    return 0;
}
