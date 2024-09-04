#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int file_open(int fd char* argv[]) {

    if(argc > 2) {
        fprintf(stderr, "USage : %s <file>\n", argv[0]);
    }

    if((fd=open(argv[1], O_RDONLY)) == -1) {
        perror("file is not open");
        exit(1);
    }

    return

}

int main(int argc, char* argv[]) {
    int fd;

    return (0);
}
