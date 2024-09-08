#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <stdarg.h>
#include <fcntl.h>
#include <pthread.h>

#define DEFAULT_TCP_PORT 5100
#define MAX_EVENTS 32
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// SmartPtr 구조체 및 함수 선언
typedef struct SmartPtr {
    void *ptr;
    int *ref_count;
    pthread_mutex_t *mutex;
} SmartPtr;

SmartPtr create_smart_ptr(void *ptr);
void retain(SmartPtr *sp);
void release(SmartPtr *sp);

// 스마트 포인터 클라이언트 관리 함수
typedef struct {
    int client_fd;
    int client_id;
    pthread_mutex_t *client_mutex; // 클라이언트 별 뮤텍스
} ClientInfo;

// 클라이언트 정보 스마트 포인터 배열
SmartPtr client_infos[MAX_CLIENTS];

// 서버 함수 선언
int create_network_tcp_process(int num_tcp_proc, ...);
void setnonblocking(int fd);
void broadcast_message(int sender_fd, char *message, int message_len, int max_clients, int is_server);

int main() {
    printf("VEDA MULTI-CLIENT CHAT PROGRAM (with SmartPtr)\n");
    usleep(1000);
    create_network_tcp_process(1, "127.0.0.1", DEFAULT_TCP_PORT);
    return 0;
}

// Global mutex for shared resources
pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;

void safe_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    pthread_mutex_lock(&global_mutex);  // Lock the mutex
    vprintf(format, args);  // Print the message
    pthread_mutex_unlock(&global_mutex);  // Unlock the mutex
    va_end(args);
}

// Smart pointer 생성 함수 구현
SmartPtr create_smart_ptr(void *ptr) {
    SmartPtr sp;
    sp.ptr = ptr;
    sp.ref_count = (int *)malloc(sizeof(int));
    if (sp.ref_count == NULL) {
        perror("Failed to allocate memory for ref_count");
        exit(EXIT_FAILURE);
    }
    *(sp.ref_count) = 1;
    sp.mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if (sp.mutex == NULL) {
        perror("Failed to allocate memory for mutex");
        free(sp.ref_count);
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(sp.mutex, NULL);
    return sp;
}

// Smart pointer retain 함수 구현
void retain(SmartPtr *sp) {
    pthread_mutex_lock(sp->mutex);
    (*(sp->ref_count))++;
    pthread_mutex_unlock(sp->mutex);
}

// Smart pointer release 함수 구현
void release(SmartPtr *sp) {
    int should_free = 0;
    pthread_mutex_lock(sp->mutex);
    (*(sp->ref_count))--;
    if (*(sp->ref_count) == 0) {
        should_free = 1;
    }
    pthread_mutex_unlock(sp->mutex);

    if (should_free) {
        free(sp->ptr);
        free(sp->ref_count);
        pthread_mutex_destroy(sp->mutex);
        free(sp->mutex);
    }
}

void setnonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        exit(EXIT_FAILURE);
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        exit(EXIT_FAILURE);
    }
}

// 클라이언트 관리 및 서버 코드 수정
int create_network_tcp_process(int num_tcp_proc, ...) {
    va_list args;
    va_start(args, num_tcp_proc);

    for (int i = 0; i < num_tcp_proc; i++) {
        int ssock, epfd, nfds = 0, client_count = 1;
        socklen_t clen;
        struct sockaddr_in servaddr, cliaddr;
        struct epoll_event ev, events[MAX_EVENTS];
        char buffer[BUFFER_SIZE];
        char client_ip[INET_ADDRSTRLEN];

        const char *ip_address = va_arg(args, const char*);
        int port = va_arg(args, int);

        // 서버 소켓 생성
        if ((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket()");
            return -1;
        }

        // 소켓 재사용 설정
        int enable = 1;
        if (setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
            perror("setsockopt(SO_REUSEADDR) failed");
            return -1;
        }

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(port);

        // 소켓 바인딩
        if (bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
            perror("bind()");
            return -1;
        }

        // 클라이언트 연결 대기
        if (listen(ssock, 8) < 0) {
            perror("listen()");
            return -1;
        }

        printf("서버가 클라이언트의 연결을 기다립니다...\n");

        epfd = epoll_create(MAX_EVENTS);
        if (epfd == -1) {
            perror("epoll_create");
            return -1;
        }

        ev.events = EPOLLIN;
        ev.data.fd = ssock;

        if (epoll_ctl(epfd, EPOLL_CTL_ADD, ssock, &ev) == -1) {
            perror("epoll_ctl: listen_sock");
            return -1;
        }

        // 서버의 표준 입력(STDIN)을 epoll에 추가
        ev.events = EPOLLIN;
        ev.data.fd = STDIN_FILENO;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {
            perror("epoll_ctl: stdin");
            return -1;
        }

        while (1) {
            nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
            for (int i = 0; i < nfds; i++) {
                if (events[i].data.fd == ssock) {
                    // 새 클라이언트 연결 처리
                    clen = sizeof(cliaddr);
                    int csock = accept(ssock, (struct sockaddr *)&cliaddr, &clen);
                    if (csock > 0) {
                        inet_ntop(AF_INET, &cliaddr.sin_addr, client_ip, INET_ADDRSTRLEN);
                        printf("[ 클라이언트 %d가 연결되었습니다. IP: %s ]\n", client_count, client_ip);

                        // 각 클라이언트에 대한 뮤텍스 생성
                        pthread_mutex_t *client_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
                        pthread_mutex_init(client_mutex, NULL);

                        setnonblocking(csock);
                        ev.events = EPOLLIN | EPOLLET;
                        ev.data.fd = csock;

                        if (epoll_ctl(epfd, EPOLL_CTL_ADD, csock, &ev) == -1) {
                            perror("epoll_ctl: add");
                            return -1;
                        }

                        // 클라이언트 소켓 및 ID 스마트 포인터로 관리
                        ClientInfo *client_info = (ClientInfo *)malloc(sizeof(ClientInfo));
                        client_info->client_fd = csock;
                        client_info->client_id = client_count++;
                        client_info->client_mutex = client_mutex; // 클라이언트 별 뮤텍스

                        client_infos[csock] = create_smart_ptr(client_info);

                        // 클라이언트 시작 시 뮤텍스 호출
                        pthread_mutex_lock(client_info->client_mutex);
                        printf("뮤텍스 %d 호출\n", client_info->client_id);
                        pthread_mutex_unlock(client_info->client_mutex);
                    }
                } 
                
                else if (events[i].data.fd == STDIN_FILENO) {
                    // 서버 입력 처리
                    fgets(buffer, BUFFER_SIZE, stdin);

                    if (strncmp(buffer, "exit", 4)==0) {
                        printf("서버 종료\n");
                        break;
                    }

                    if (strncmp(buffer, "...", 3) ==0) {
                        close(ssock);
                        exit(1);
                        break;
                    }

                    if(strlen(buffer) < 0) {
                        buffer[strlen(buffer) - 1] = '\0';
                    }

                    if (strlen(buffer) > 0) {
                        broadcast_message(-1, buffer, strlen(buffer), MAX_CLIENTS, 1);
                        buffer[strcspn(buffer, "\n")] = 0;
                        printf("[내가 보낼 메시지]: ");
                        printf("%s\n", buffer);
                    }
                } 
                
                else if (events[i].events & EPOLLIN) {
                    // 클라이언트로부터 메시지 수신 처리
                    int csock = events[i].data.fd;
                    memset(buffer, 0, BUFFER_SIZE);
                    int n = read(csock, buffer, BUFFER_SIZE);

                    if (n > 0) {
                        ClientInfo *client_info = (ClientInfo *)client_infos[csock].ptr;
                        printf("[ 클라이언트 %d 메세지 ]: %s", client_info->client_id, buffer);
                        broadcast_message(csock, buffer, n, MAX_CLIENTS, 0);
                    } else if (n == 0) {
                        // 클라이언트가 종료될 때 처리
                        ClientInfo *client_info = (ClientInfo *)client_infos[csock].ptr;
                        printf("클라이언트 %d번 연결이 종료되었습니다. 소켓: %d번\n", client_info->client_id, csock);

                        // 클라이언트 종료 시 뮤텍스 잠금
                        pthread_mutex_lock(client_info->client_mutex);
                        printf("뮤텍스 %d 잠금\n", client_info->client_id);
                        pthread_mutex_unlock(client_info->client_mutex);

                        close(csock);
                        events[i].data.fd = -1;
                        release(&client_infos[csock]);
                    }
                }

            }
        }

        va_end(args);
        close(ssock);
    }
    return 0;
}

// 메시지 브로드캐스트
void broadcast_message(int sender_fd, char *message, int message_len, int max_clients, int is_server) {
    char broadcast_message[BUFFER_SIZE + 50];

    if (is_server) {
        // 서버에서 보낸 메시지
        snprintf(broadcast_message, sizeof(broadcast_message), "[서버 메시지]: %s", message);
    } else {
        // 클라이언트에서 보낸 메시지
        ClientInfo *client_info = (ClientInfo *)client_infos[sender_fd].ptr;
        snprintf(broadcast_message, sizeof(broadcast_message), "[클라이언트 %d 메시지]: %s", client_info->client_id, message);
    }

    // 모든 클라이언트에게 전송
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_infos[i].ptr != NULL && ((ClientInfo *)client_infos[i].ptr)->client_fd != sender_fd) {
            write(((ClientInfo *)client_infos[i].ptr)->client_fd, broadcast_message, strlen(broadcast_message));
        }
    }
}