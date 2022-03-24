#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
    int pfd[2];
    pipe(pfd);
    pid_t p = fork();
    // 0 -> read end
    // 1 -> write end
    if(p>0){
        // parent
        close(pfd[1]);
        char buff[100];
        read(pfd[0], buff, 100);
        printf("%s", buff);
    }else{
        // child
        close(pfd[0]);
        write(pfd[1], "Hello pipes from the child\n", strlen("Hello pipes from the child\n"));
    }
    system("rm -f a.out");
    return 0;
}