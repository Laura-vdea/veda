#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // 원본 파일이 존재하는지 확인
    if (access("flock.txt", F_OK) == -1) {
        perror("flock.txt does not exist");
        exit(1);
    }

    // 하드 링크 생성
    if (link("flock.txt", "hlink.txt") == -1) {
        perror("link");
        exit(1);
    }

    // 파일 목록 출력
    system("ls -l flock.txt hlink.txt");

    // 심볼릭 링크 생성
    if (symlink("flock.txt", "slink.txt") == -1) {
        perror("symlink");
        exit(1);
    }

    // 모든 파일 목록 출력
    system("ls -al flock.txt hlink.txt slink.txt");

    return 0;
}
