#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
    pid_t c = fork();
    if(c==0){
        // this will be executed in the child process
        // compile and create the p2 executable file just incase it doesn't exist
        system("gcc ./p2.c -o p2");
        char **args;
        // execute p2 executable in the child process
        // sleep(2);
        execv("./p2", args);
    }else{
        // this will be executed in the parent process
        printf("Hello from p1!\n");
        wait(NULL);
        printf("Child exited\n");
    }
    // cleaning up the folder
    system("rm -f p2 a.out");
    return 0;
}