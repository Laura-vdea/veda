#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int msgid;
    struct msgbuf {
        long mtype;
        char mtext[1014];
    } mbuf;

    msgid = msgget(ftok("/home/ubuntu", 100), IPC_CREAT | 0666);
    if(msgid==-1) {
        perror("msgget");
        exit(1);
    }

    if(msgrcv(msgid, &mbuf, sizeof(mbuf), 1, 0)<0) {
        printf("error\n");
        exit(1);
    }
    sleep(5);
    msgctl(msgid, IPC_RMID, 0);
}
