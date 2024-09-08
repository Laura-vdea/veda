#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // 소켓 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n 소켓 생성 오류 \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 서버 주소 설정
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\n 잘못된 주소 \n");
        return -1;
    }

    // 서버에 연결
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n 연결 실패 \n");
        return -1;
    }

    while (1) {
        printf("메시지를 입력하세요: ");
        fgets(buffer, 1024, stdin);

        send(sock, buffer, strlen(buffer), 0);
        printf("메시지를 보냈습니다.\n");

        int valread = read(sock, buffer, 1024);
        buffer[valread] = '\0';
        printf("서버로부터 응답: %s\n", buffer);
    }

    return 0;
}