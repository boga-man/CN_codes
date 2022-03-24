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

key_t key = ftok("./msgqueue", 65);

int leftPid, rightPid;

int circSigCnt = 3, revCircSigCnt = 3;

void leftPidHandler(int sig, siginfo_t *info, void *context)
{
    cout << "Received signal from P1\n";
    leftPid = info->si_pid;
}

void circSigHandler(int sig)
{
    circSigCnt--;
    cout << "Reached P2\n";
    if (circSigCnt >= 0)
    {
        kill(rightPid, SIGUSR1);
    }
}

void revCircSigHandler(int sig)
{
    revCircSigCnt--;
    cout << "Reached P2(Reverse)\n";
    if (revCircSigCnt >= 0)
    {
        kill(leftPid, SIGUSR2);
    }
}

int main()
{
    int mqId = msgget(key, 0666 | IPC_CREAT);
    printf("My PID is %d\n", getpid());
    string pid = to_string(getpid());
    msgBuff p2pid;
    p2pid.type = 2;
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

    // sending the pid of P1 into the message queue
    msgBuff p1pid;
    p1pid.type = 1;
    strcpy(p1pid.msg, to_string(leftPid).c_str());
    // putting the pid of P1 in the message queue
    msgsnd(mqId, &p1pid, sizeof(p1pid), 0);

    // sending the acknowledgment to p3
    kill(rightPid, SIGUSR1);

    cout << "Left PID = " << leftPid << " Right PID = " << rightPid << endl;

    // adding the circular signalling handler
    signal(SIGUSR1, circSigHandler);
    while (circSigCnt)
    {
        pause();
    }

    // adding the reverse circular signalling handler
    signal(SIGUSR2, revCircSigHandler);
    while (revCircSigCnt)
    {
        pause();
    }
}