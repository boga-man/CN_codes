#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
using namespace std;

struct msgBuff{
    long type;
    char msg[128];
};

int leftPid, rightPid;

int circSigCnt = 3, revCircSigCnt = 3;

key_t key = ftok("msgqueue", 65);

void leftPidHandler(int sig, siginfo_t *info, void *context){
    cout<<"Received signal from P4\n";
    leftPid = info->si_pid;
}

void circSigHandler(int sig){
    circSigCnt--;
    cout<<"Reached P1\n";
    if(circSigCnt>0){
        kill(rightPid, SIGUSR1);
    }
}

void revCircSigHandler(int sig){
    revCircSigCnt--;
    cout<<"Reached P1(Reverse)\n";
    if(revCircSigCnt>0){
        kill(leftPid, SIGUSR2);
    }
}

int main(){
    system("g++ p2.cpp -o p2");
    system("g++ p3.cpp -o p3");
    system("g++ p4.cpp -o p4");
    cout<<"Created the executables\n";
    printf("My PID is %d\n", getpid());
    int mqId = msgget(key, 0666|IPC_CREAT);
    msgBuff msg;
    // receiving the message from p2
    msgrcv(mqId, &msg, sizeof(msg), 2, 0);
    int p2pid;
    sscanf(msg.msg, "%d", &p2pid);
    // recording the pid of right side process into rightPid
    rightPid = p2pid;
    // sending the acknowledgment to p2 on receiving the message
    kill(rightPid, SIGUSR1);

    // receive signal from p4
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = leftPidHandler;
    sigaction(SIGUSR1, &act, NULL);
    pause();

    cout<<"Left PID = "<<leftPid<<" Right PID = "<<rightPid<<endl;

    // circular signalling starts here
    cout<<"Starting the circular signalling for 3 times\n";
    kill(rightPid, SIGUSR1);
    // adding the circular signalling handler
    signal(SIGUSR1, circSigHandler);
    // waiting for the signals
    while(circSigCnt){
        pause();
    }
    // adding the reverse circular signalling handler
    signal(SIGUSR2, revCircSigHandler);
    // reverse circular signalling starts here
    kill(leftPid, SIGUSR2);
    // waiting for the signals
    while(revCircSigCnt){
        pause();
    }
}