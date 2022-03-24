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
    cout<<"Received signal from P3\n";
    leftPid = info->si_pid;
}

int main(){
    int mqId = msgget(key ,0666 | IPC_CREAT);
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
    cout<<"p3pid (left): "<<leftPid<<endl;

    // p4 sends the message with p4pid and type 18
    msgBuff msg;
    msg.type = 18;
    sprintf(msg.msg, "%d", getpid());
    msgsnd(mqId, &msg, sizeof(msg), 0);

    // receive the pid of P1 from message queue
    // msgBuff msg;
    msgrcv(mqId, &msg, sizeof(msg), 1, 0);
    int p1pid;
    sscanf(msg.msg, "%d", &p1pid);
    // recording the pid of right side process into rightPid
    rightPid = p1pid;
    cout<<"p1pid (right): "<<rightPid<<endl;
    // seding the acknowledgment to p4
    kill(rightPid, SIGUSR1);

}