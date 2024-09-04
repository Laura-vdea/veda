#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int fd;

    if(argc!=2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        exit(1);
    }

    if((fd=open(argv[1], O_RDONLY)) == -1) {
        perror("open");
        exit(1);
    }

    printf("fd :  %d\n", fd);

    char buf[1024];
    int size;

    if((size=read(fd, buf, 1023)) == -1) {
        perror("read");
        exit(1);
    }

    buf[size]='\0';

    int ofd;

    if((ofd=open("test.c", O_WRONLY|O_CREAT|O_TRUNC, 0777)) == -1) {
        perror("open");
        exit(1);
    }

    write(ofd, buf, size);

    close(fd);
    close(ofd);

    return (0);
}
