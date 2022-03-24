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

int mqId = msgget(key ,0666 | IPC_CREAT);
    printf("My PID is %d\n", getpid());
    string pid = to_string(getpid());
    msgBuff p4pid;
    p4pid.type = 4;
    strcpy(p4pid.msg, pid.c_str());
    // putting the message into the message queue
    msgsnd(mqId, &p4pid, sizeof(p4pid), 0);

key_t key = ftok("./msgqueue", 65);

int leftPid, rightPid;
int circSigCnt = 3, revCircSigCnt = 3;


void leftPidHandler(int sig, siginfo_t *info, void *context){
    cout<<"Received signal from P3\n";
    leftPid = info->si_pid;
}

void circSigHandler(int sig){
    circSigCnt--;
    cout<<"Reached P4\n";
    if(circSigCnt>=0){
        kill(rightPid, SIGUSR1);
    }
}

void revCircSigHandler(int sig){
    revCircSigCnt--;
    cout<<"Reached P4(Reverse)\n";
    if(revCircSigCnt>=0){
        kill(leftPid, SIGUSR2);
    }
}

int main(){
    int mqId = msgget(key ,0666 | IPC_CREAT);
    printf("My PID is %d\n", getpid());
    string pid = to_string(getpid());
    msgBuff p4pid;
    p4pid.type = 4;
    strcpy(p4pid.msg, pid.c_str());
    // putting the message into the message queue
    msgsnd(mqId, &p4pid, sizeof(p4pid), 0);

    // receiving the signal from P3
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = leftPidHandler;
    sigaction(SIGUSR1, &act, NULL);
    pause();

    // receive the pid of P1 from message queue
    msgBuff msg;
    msgrcv(mqId, &msg, sizeof(msg), 1, 0);
    int p1pid;
    sscanf(msg.msg, "%d", &p1pid);
    // recording the pid of right side process into rightPid
    rightPid = p1pid;
    // seding the acknowledgment to p4
    kill(rightPid, SIGUSR1);

    cout<<"Left PID = "<<leftPid<<" Right PID = "<<rightPid<<endl;

    // adding the circular signalling handler
    signal(SIGUSR1, circSigHandler);
    while(circSigCnt){
        pause();
    }

    // adding the reverse circula signalling handler
    signal(SIGUSR2, revCircSigHandler);
    while(revCircSigCnt){
        pause();
    }
    
}