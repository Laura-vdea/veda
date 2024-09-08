#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 5100

int main(int argc, char *argv[]) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024];
    fd_set readfds;
    int max_sd;

    // 명령행 인수 확인 (IP 주소 입력 여부 확인)
    if (argc != 2) {
        printf("사용법: %s <서버 IP 주소>\n", argv[0]);
        return -1;
    }

    // 소켓 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("소켓 생성 오류\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 서버 주소 설정 (명령행 인수로 받은 IP 주소 사용)
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        printf("잘못된 주소\n");
        return -1;
    }

    // 서버에 연결
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("연결 실패\n");
        return -1;
    }

    printf("서버와 연결되었습니다.\n");
    
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);  // 표준 입력 감시 (키보드 입력)
        FD_SET(sock, &readfds);  // 서버 소켓 감시
        max_sd = sock;

        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("select()");
            break;
        }

        // 사용자 입력 감지
        if (FD_ISSET(0, &readfds)) {
            fgets(buffer, 1024, stdin);

            if (strncmp(buffer, "exit", 4) == 0) {
                printf("채팅 종료\n");
                close(sock);  // 소켓을 닫고 종료
                break;
            }

            if (strncmp(buffer, "...", 3) == 0) {
                close(sock);
                printf("클라이언트 연결 종료\n");
                break;
            }

            send(sock, buffer, strlen(buffer), 0);

            if (strlen(buffer) > 0) {
                printf("[내가 보낸 메시지]: %s", buffer);
            }
        }

        // 서버로부터 메시지 수신
        if (FD_ISSET(sock, &readfds)) {
            int valread = read(sock, buffer, 1024);
            if (valread <= 0) {
                printf("서버 연결이 종료되었습니다.\n");
                break;
            }

            buffer[valread] = '\0';
            printf("%s", buffer);  // 서버로부터 받은 메시지를 그대로 출력
        }
    }

    close(sock);
    return 0;
}
