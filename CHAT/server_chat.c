#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 8080
#define MAX_CLIENTS 5

int main() {
    int server_fd, new_socket, client_sockets[MAX_CLIENTS], max_sd, sd;
    struct sockaddr_in address;
    fd_set readfds;
    int addrlen = sizeof(address);
    char buffer[1024];

    // 초기화
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }

    // 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 듣기 모드로 전환
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("서버가 준비되었습니다. 연결을 기다립니다...\n");

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // 클라이언트 소켓을 셋에 추가
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sockets[i];
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // select를 사용하여 소켓의 상태를 검사
        if (select(max_sd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select error");
            exit(EXIT_FAILURE);
        }

        // 새 클라이언트 연결 수락
        if (FD_ISSET(server_fd, &readfds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            printf("새로운 연결, 소켓 FD는 %d, IP는 %s, PORT는 %d\n",
                   new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // 클라이언트 목록에 추가
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    printf("클라이언트 소켓을 리스트에 추가: %d\n", i);
                    break;
                }
            }
        }

        // 클라이언트 소켓에서 데이터 읽기
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sockets[i];
            if (FD_ISSET(sd, &readfds)) {
                int valread = read(sd, buffer, 1024);
                if (valread == 0) {
                    // 클라이언트 연결 종료
                    getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    printf("클라이언트 연결 종료, IP %s, PORT %d\n",
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    close(sd);
                    client_sockets[i] = 0;
                } else {
                    // 메시지 수신
                    buffer[valread] = '\0';
                    printf("클라이언트로부터 메시지: %s\n", buffer);
                    send(sd, "메시지 수신 완료\n", strlen("메시지 수신 완료\n"), 0);
                }
            }
        }
    }

    return 0;
}
