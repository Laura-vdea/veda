#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdlib.h>

int main()
{
    char ch;
    struct termios buf, old_buf;

    if(ioctl(0, TCGETS, &buf) == -1) {
        perror("ioctl");
        exit(1);
    }

    old_buf = buf;  // Save current settings

    buf.c_lflag &= ~ICANON; // disable canonical mode and echoing
    buf.c_lflag &= ~ECHO;
    buf.c_cc[VMIN] = 1;  // Set min input chars
    buf.c_cc[VTIME] = 0; // Immediate return on input

    if(ioctl(0, TCSETS, &buf) == -1) {
        perror("ioctl");
        exit(1);
    }

    while(1) {
        ch = getchar();
//        if(ch == 'q') {
        if(ch == '\n' || ch == '\r') {
            break;
        }
        printf("%x ", ch);  // Print character in hexadecimal
    }

    if(ioctl(0, TCSETS, &old_buf) == -1) {
        perror("ioctl");
        exit(1);
    }

    return 0;
}
