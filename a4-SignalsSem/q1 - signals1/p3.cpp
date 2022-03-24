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

key_t key = ftok("./msgqueue", 65);

int leftPid, rightPid;
int circSigCnt = 3, revCircSigCnt = 3;

void leftPidHandler(int sig, siginfo_t *info, void *context){
    cout<<"Received signal from P2\n";
    leftPid = info->si_pid;
}

void circSigHandler(int sig){
    circSigCnt--;
    cout<<"Reached P3\n";
    if(circSigCnt>=0){
        kill(rightPid, SIGUSR1);
    }
}

void revCircSigHandler(int sig){
    revCircSigCnt--;
    cout<<"Reached P3(Reverse)\n";
    if(revCircSigCnt>=0){
        kill(leftPid, SIGUSR2);
    }
}

int main(){
    int mqId = msgget(key ,0666 | IPC_CREAT);
    printf("My PID is %d\n", getpid());
    string pid = to_string(getpid());
    msgBuff p3pid;
    p3pid.type = 3;
    strcpy(p3pid.msg, pid.c_str());
    // putting the message into the message queue
    msgsnd(mqId, &p3pid, sizeof(p3pid), 0);

    // receiving the signal from P2
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = leftPidHandler;
    sigaction(SIGUSR1, &act, NULL);
    pause();

    // receving the message from p4
    msgBuff msg;
    msgrcv(mqId, &msg, sizeof(msg), 4, 0);
    int p4pid;
    sscanf(msg.msg, "%d", &p4pid);
    // recording the pid of right side process into rightPid
    rightPid = p4pid;
    // seding the acknowledgment to p4
    kill(rightPid, SIGUSR1);

    cout<<"Left PID = "<<leftPid<<" Right PID = "<<rightPid<<endl;

    // adding the circular signalling handler
    signal(SIGUSR1, circSigHandler);
    while(circSigCnt){
        pause();
    }

    // adding the reverse circular signalling handler
    signal(SIGUSR2, revCircSigHandler);
    while(revCircSigCnt){
        pause();
    }
}