#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    printf("Hello dup\n");

    int fd;
    fd=open("dup.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd == -1) {
        perror("open");
        exit(1);
    }
    // close(1);
    //dup(fd);

    dup2(fd, 1);
    printf("Hello dup2\n");

    return (0);
}
