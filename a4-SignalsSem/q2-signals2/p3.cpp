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

key_t key = ftok("groupMsg", 65);

int leftPid, rightPid;

void leftPidHandler(int sig, siginfo_t *info, void *context){
    cout<<"Received signal from P2\n";
    leftPid = info->si_pid;
}

void handler(int sig){
    cout<<"\n";
}

int main(){
    int mqId = msgget(key ,0666 | IPC_CREAT);
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
    cout<<"p2pid (left): "<<leftPid<<endl;

    // p3 sends the message with type 18 and p3pid for grouping
    msgBuff msg;
    msg.type = 18;
    sprintf(msg.msg, "%d", getpid());
    msgsnd(mqId, &msg, sizeof(msg), 0);

    // receving the message from p4
    // msgBuff msg;
    msgrcv(mqId, &msg, sizeof(msg), 4, 0);
    int p4pid;
    sscanf(msg.msg, "%d", &p4pid);
    // recording the pid of right side process into rightPid
    rightPid = p4pid;
    cout<<"p4pid (right): "<<rightPid<<endl;
    
    // sending the acknowledgment to p4
    kill(rightPid, SIGUSR1);
    // signal(SIGUSR1, handler);
}