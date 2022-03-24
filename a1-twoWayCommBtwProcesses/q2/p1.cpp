#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
using namespace std;

int main(){
    // int fd[2];
    // int fd2[2];
    // pipe(fd);
    // pipe(fd2);
    // // cout<<fd[0]<<" "<<fd[1]<<"\n";
    // // cout<<fd2[0]<<" "<<fd2[1]<<"\n";
    // int pid = fork();
    // if(pid == 0){
    //     close(fd[0]);
    //     close(fd2[1]);
    //     int stdin = dup(0);
    //     int stdout = dup(1);
    //     // cout<<"child std in and out"<<stdin<<" "<<stdout<<endl;
    //     dup2(fd[1], 1);
    //     dup2(fd2[0], 0);
    //     char buff[3];
    //     buff[0] = stdin;
    //     buff[1] = stdout;
    //     buff[2] = '\0';
    //     char *args[3];
    //     string file = "./p2";
    //     strcpy(args[0], file.c_str());
    //     args[1] = buff;
    //     args[2] = NULL;
    //     execv("./p2", args);
    // }
    // else{
    //     close(fd[1]);
    //     close(fd2[0]);
    //     string msg;
    //     int stdin = dup(0);
    //     int stdout = dup(1);
    //     // cout<<"parent std in and out"<<stdin<<" "<<stdout<<endl;
    //     dup2(fd2[1], 1);
    //     dup2(fd[0], 0);
    //     // char *pid_str;
    //     // sprintf(pid_str, "%d", getpid());
    //     // write(stdout, pid_str, strlen(pid_str));
    //     cout<<"parent\n";
    //     cin>>msg;
    //     write(stdout, msg.c_str(), msg.length());
    // }
    // return 0;
    int fd[2];
    int fd2[2];
    pipe(fd);
    pipe(fd2);
    int pid = fork();
    if(pid == 0){
        // child process
        cout<<"In child\n"; 
        close(fd[0]);
        close(fd2[1]);
        int stdin = dup(0);
        int stdout = dup(1);
        // cout<<stdin<<" "<<stdout<<endl;
        dup2(fd[1], 1);
        dup2(fd2[0], 0);
        char buff[3];
        buff[0] = stdin;
        buff[1] = stdout;
        buff[2] = '\0';
        char *args[3];
        string file = "./p2";
        strcpy(args[0], file.c_str());
        args[1] = buff;
        args[2] = NULL;
        system("g++ ./p2.cpp -o p2");
        execv("p2", args);
    }else{
        // parent process
        cout<<"In parent\n";
        close(fd[1]);
        close(fd2[0]);
        int stdin = dup(0);
        int stdout = dup(1);
        dup2(fd[0], 0);
        dup2(fd2[1], 1);
        cout<<"Hello from parent\n"; // into fd2 pipe
        char buff[100];
        int bytes = read(0, buff, 100); // waiting for data because no data in fd
        write(stdout, buff, strlen(buff));
    }
    // cleaning up the folder
    system("rm -f p2 a.out");
    return 0;
}