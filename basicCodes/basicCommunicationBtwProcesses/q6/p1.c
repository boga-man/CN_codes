#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
    // communication between parent and child process without dup2
    int fd[2];
    pipe(fd);
    int pid = fork();
    if(pid == 0){
        // child process
        close(fd[0]);
        dup2(fd[1], 1);
        system("gcc ./p2.c -o p2");
        execv("p2", NULL);
    }else{
        // parent process
        close(fd[1]);
        dup2(fd[0], 0);
        char buff[100];
        int bytes = read(0, buff, 100);
        printf("%s\n", buff);
    }
    // cleaning up the folder
    system("rm -f p2 a.out");
    return 0;
}