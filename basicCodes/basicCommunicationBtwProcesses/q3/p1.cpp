#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
    // forking first child
    pid_t c1 = fork();
    // forking second child
    pid_t c2;
    char **args;
    if(c1==0){
        // this will be executed in the first child process
        // compile and create the p2 executable file just incase it doesn't exist
        printf("In child 1\n");
        system("gcc ./p2.c -o p2");
        // execute p2 executable in the child process
        execv("./p2", args);
    }else{
        c2 = fork();
        if(c2==0){
            printf("In child 2\n");
            system("gcc ./p3.c -o p3");
            execv("./p3", args);
        }
        // this will be executed in the parent process
        if(c2>0){
            printf("Hello from p1!\n");
            waitpid(c1, NULL, 0);
            waitpid(c2, NULL, 0);
            printf("Children exited\n");

        }
    }
    // cleaning up the folder
    system("rm -f p2 a.out");
    system("rm -f p3 a.out");
    return 0;
}
