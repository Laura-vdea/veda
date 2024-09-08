#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv) {
    // epoll을 위한 변수 선언
    int n;  // epoll_wait 함수의 반환값을 저장할 변수
    int epfds;  // epoll 파일 디스크립터
    char buf[1024];  // 데이터를 저장할 버퍼
    struct epoll_event ev, evlist[3];  // epoll 이벤트 구조체와 이벤트 목록 배열 (최대 3개의 이벤트를 처리)

    // epoll 인스턴스를 생성 (커널의 epoll 서브시스템에서 핸들 생성)
    epfds = epoll_create(1);  // 1은 파라미터로 최소한의 값으로 사용됨
    if(epfds == -1) {  // epoll 인스턴스 생성 실패 시
        perror("epoll_create");  // 오류 메시지 출력
        exit(1);  // 프로그램 종료
    }

    // 표준 입력(0번 파일 디스크립터)을 비차단 모드로 설정 (non-blocking)
    fcntl(0, F_SETFL, O_NONBLOCK);  // 0은 표준 입력을 가리키며, F_SETFL로 파일 상태 플래그를 설정

    // epoll 이벤트 설정
    ev.events = EPOLLIN | EPOLLET;  // EPOLLIN: 읽기 가능 이벤트, EPOLLET: 엣지 트리거 모드 설정
    ev.data.fd = 0;  // 표준 입력을 가리킴 (fd 0)

    // epoll 인스턴스에 표준 입력(0)을 등록
    if(epoll_ctl(epfds, EPOLL_CTL_ADD, 0, &ev) == -1) {  // EPOLL_CTL_ADD로 epoll에 파일 디스크립터 추가
        perror("epoll_ctl\n");  // 오류 발생 시 메시지 출력
        exit(1);  // 프로그램 종료
    }

    // 메인 이벤트 루프
    while(1) {
        memset(buf, 0, sizeof(buf));  // 버퍼를 초기화 (이전에 읽은 데이터를 지움)
        
        // epoll_wait로 이벤트가 발생할 때까지 대기
        n = epoll_wait(epfds, evlist, 3, 500);  // epoll 이벤트 대기, 최대 3개의 이벤트를 500ms 동안 감시
        printf("n = %d\n", n);  // 감지된 이벤트의 수를 출력
        
        // 발생한 이벤트 처리
        for(int i = 0; i<n; i++) {  // n개의 이벤트 처리
            if(evlist[i].events == EPOLLIN) {  // 이벤트가 EPOLLIN (읽기 가능)일 때
                // 데이터를 처리할 부분 (현재는 주석 처리됨)
                // fgets(buf, 1024, stdin);  // 표준 입력으로부터 데이터를 읽음 (주석 처리됨)
                // buf[strlen(buf) - 1] = '\0';  // 읽어온 데이터의 끝에 NULL 문자를 추가 (주석 처리됨)
                
                // 수신된 데이터 출력 (현재 버퍼는 초기화된 상태로 출력)
                printf("Recieved data: %s\n", buf);  // 버퍼에 수신된 데이터를 출력
            }
        }
    }
}
