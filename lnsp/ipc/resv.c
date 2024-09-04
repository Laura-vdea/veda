#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd;
    fd = open("mypipe", O_RDONLY);
    if(fd == -1){
        perror("open");
        exit(1);
    }
    char buf[512];
    int n;
    for(int i=0; i<5; i++) {
        n = read(fd, buf, sizeof(buf));
        buf[n]='\0';
        printf("from send > %s\n", buf);
    }
    sleep(1);
    close(fd);
}
