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

struct msgBuff
{
    long type;
    char msg[128];
};

key_t key = ftok("groupMsg", 65);

int leftPid, rightPid;

void leftPidHandler(int sig, siginfo_t *info, void *context)
{
    cout << "Received signal from P1\n";
    leftPid = info->si_pid;
}

int main()
{
    cout<<getpid();
    int mqId = msgget(key, 0666 | IPC_CREAT);
    msgBuff p2pid;
    p2pid.type = 2;
    string pid = to_string(getpid());
    strcpy(p2pid.msg, pid.c_str());
    // putting the message into the message queue
    msgsnd(mqId, &p2pid, sizeof(p2pid), 0);
    // on receiving the SIGUSR1 from p1, record its pid as leftPid
    struct sigaction act;
    act.sa_sigaction = leftPidHandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);
    pause();

    msgBuff msg;
    // receiving the message from p3
    msgrcv(mqId, &msg, sizeof(msg), 3, 0);
    int p3pid;
    sscanf(msg.msg, "%d", &p3pid);
    // recording the pid of right side process into rightPid
    rightPid = p3pid;

    // remnant of previous execution is remaining in the queue
    // so removing it
    // msgrcv(mqId, &msg, sizeof(msg), 1, 0);
    // cout<<"Remnant: "<<msg.msg<<endl;
    // p2 sends two messages, one with (p1pid, type = 1) and other with (p2pid, type = 18)
    msg.type = 1;
    // sprintf(msg.msg, "%d", leftPid);
    strcpy(msg.msg, to_string(leftPid).c_str());
    cout<<"p1pid (left): "<<leftPid<<endl;
    // sending the message containing p1pid and type = 1
    msgsnd(mqId, &msg, sizeof(msg), 0);

    msg.type = 18;
    sprintf(msg.msg, "%d", getpid());
    // sending the msg containing p2pid and type = 18
    msgsnd(mqId, &msg, sizeof(msg), 0);

    // sending the acknowledgment to p3
    cout<<"p3pid (right): "<<rightPid<<endl;
    kill(rightPid, SIGUSR1);



}