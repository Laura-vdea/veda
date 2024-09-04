#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int file_lock(int fd);
int file_unlock(int fd);

int main() {
    int fd;
    int count = 0;

    if((fd=open("rlock.txt", O_CREAT | O_WRONLY, 0644))  == 01) {
        perror("open");
        exit(1);
    }

    char buf[16];

    for(int i=0; i<10; i++) {
        memset(buf, 0, 16);
        sprintf(buf, "%d", 0);
        write(fd, buf,16);
    }
    close(fd);
    return (0);
}
