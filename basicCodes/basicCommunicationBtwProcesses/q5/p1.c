#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
    int cpfd[2];
    int pcfd[2];
    pipe(cpfd);
    pipe(pcfd);
    pid_t p = fork();
    // 0 -> read end
    // 1 -> write end
    if(p>0){
        close(pcfd[0]);
        write(pcfd[1], "Hello pipes from the parent\n", strlen("Hello pipes from the parent\n"));
        char buff[100];
        close(cpfd[1]);
        read(cpfd[0], buff, 100);
        printf("%s", buff);
    }else{
        close(cpfd[0]);
        write(cpfd[1], "Hello pipes from the child", strlen("Hello pipes from the child"));
        char buff[100];
        close(pcfd[1]);
        read(pcfd[0], buff, 100);
        printf("%s", buff);
    }
    system("rm -f a.out");
    return 0;
}