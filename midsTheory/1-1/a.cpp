#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
using namespace std;

int j1pid, j2pid, j3pid;

int getPidFromPName(int i){
    string command = "pidof j" + to_string(i);
    int fd = fileno(popen(command.c_str(), "r"));
    char s[1000];
    read(fd, &s, 1000);
    int jpid = atoi(s);
    return jpid;
}

void handler(int sig){
    if(sig==SIGUSR1){
        cout<<"Asking a judge for question\n";
    }
    if(sig==SIGUSR2){
        cout<<"Asking scores from judges\n";
    }
    if(sig==SIGINT){
        // after judges have given scores, kill the remaining processes
        system("killall s p a j1 j2 j3");
        SIG_DFL(SIGINT);
    }
    return;
}

int main(){
    cout<<"Anchor is alive\n";
    signal(SIGUSR1, handler);
    signal(SIGUSR2, handler);

    // getpid of a random judge using process name and send signal to it to send question to screen
    j1pid = getPidFromPName(1);
    j2pid = getPidFromPName(2);
    j3pid = getPidFromPName(3);

    // wait for signal from screen that performance finished
    pause();

    // select a random judge and send a signal
    // int rjid = (rand()%3) + 1;
    kill(j2pid, SIGUSR1);


    // wait for signal from screen that performer had answered question
    pause();

    // signal all the judges to give scores
    kill(j1pid, SIGUSR2);
    kill(j2pid, SIGUSR2);
    kill(j3pid, SIGUSR2);

    signal(SIGINT, handler);
    pause();

    return 0;

}