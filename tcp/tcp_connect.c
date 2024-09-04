#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define TCP_PORT 5100 				/* 서버의 포트 번호 */

int main(int argc, char **argv)
{
    int ssock; 					/* 소켓 디스크립트 정의 */
    socklen_t clen;
    int n;
    struct sockaddr_in servaddr, cliaddr; 	/* 주소 구조체 정의 */
    char mesg[BUFSIZ];

    fd_set readfd;
    int maxfd, client_index, start_index;
    int client_fd[5] = {0};

    /* 서버 소켓 생성 */
    if((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    /* 주소 구조체에 주소 지정 */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(TCP_PORT); 	/* 사용할 포트 지정 */

    /* bind 함수를 사용하여 서버 소켓의 주소 설정 */
    if(bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind()");
        return -1;
    }

    /* 동시에 접속하는 클라이언트의 처리를 위한 대기 큐를 설정 */
    if(listen(ssock, 8) < 0) {
        perror("listen()");
        return -1;
    }

    FD_ZERO(&readfd);
    maxfd = ssock;
    client_index = 0;

    do {
        FD_ZERO(&readfd);  // select 전에 매번 초기화
        FD_SET(ssock, &readfd);
        for(start_index = 0; start_index < client_index; start_index++) {
            FD_SET(client_fd[start_index], &readfd);
            if(client_fd[start_index] > maxfd) {
                maxfd = client_fd[start_index];
            }
        }

        maxfd = maxfd + 1;  // select를 위한 maxfd 설정

        if(select(maxfd, &readfd, NULL, NULL, NULL) < 0) {
            perror("select()");
            return -1;
        }

        /* 클라이언트로 buf에 있는 문자열 전송 */
        if(write(ssock, mesg, n) <= 0)
            perror("write()");

        if(FD_ISSET(ssock, &readfd)) {
            clen = sizeof(struct sockaddr_in);
            /* 클라이언트가 접속하면 접속을 허용하고 클라이언트 소켓 생성 */
            int csock = accept(ssock, (struct sockaddr *)&cliaddr, &clen);
            if(csock < 0) {
                perror("accept()");
                return -1;
            } else {
                /* 네트워크 주소를 문자열로 변경 */
                inet_ntop(AF_INET, &cliaddr.sin_addr, mesg, BUFSIZ);
                printf("Client is connected : %s\n", mesg);

                FD_SET(csock, &readfd);
                client_fd[client_index] = csock;
                client_index++;
                if(client_index == 5) break;  // 최대 클라이언트 수에 도달하면 종료
                continue;
            }
        }

        for(start_index = 0; start_index < client_index; start_index++) {
            if(FD_ISSET(client_fd[start_index], &readfd)) {
                memset(mesg, 0, sizeof(mesg));

                if((n = read(client_fd[start_index], mesg, sizeof(mesg))) > 0) {
                    printf("Recived data : %s", mesg);
                    write(client_fd[start_index], mesg, n);
                    close(client_fd[start_index]);

                    FD_CLR(client_fd[start_index], &readfd);
                    client_fd[start_index] = 0;  // 클라이언트 소켓 배열에서 제거
                    client_index--;
                } else {
                    perror("read()");
                }
            }
        }
    } while(strncmp(mesg, "q", 1));  // 클라이언트로부터 "q" 수신 시 종료

    close(ssock); 			/* 서버 소켓을 닫음 */

    return 0;
}
