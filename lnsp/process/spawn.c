#include <stdio.h>
#include <sys/wait.h>
#include <spawn.h>
#include <unistd.h>
#include <stdlib.h>

extern char **environ;

int my_system(char *cmd) {
    pid_t pid;
    int status;
    pid = fork();
    printf("pid = [%d]\n", pid);
    char *argv[] = {"sh", "-c", cmd, NULL};

    if(pid < 0){
        printf("fork Error...: return is [%d] \n", pid);
        perror("fork error...");
        return -1;
    } else if(pid > 0) {
        printf("child process : [%d] - parent process : [%d] \n", pid, getpid());
        exit(0);
    } else if(pid == 0) {
        printf("process : [%d]\n", getpid());
        posix_spawn(&pid, "/bin/sh", NULL, NULL, argv, environ);
        waitpid(pid, &status, 0);
        return status;
    }
   //  posix_spawn(&pid, "/bin/sh", NULL, NULL, argv, environ);
   //  waitpid(pid, &status, 0);
  //  return status;
}

int main(int argc, char**argv, char **envp) {
    while(*envp)
        printf("%s\n", *envp++);

    my_system("who");
    my_system("nocommand");
    my_system("cal");

    return (0);
}
