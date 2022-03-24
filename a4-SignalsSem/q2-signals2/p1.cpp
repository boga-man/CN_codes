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

void leftPidHandler(int sig, siginfo_t *info, void *context){
    cout<<"Received signal from P4\n";
    leftPid = info->si_pid;
}

int main()
{
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

    // creating a group with all the pids in the message queue of type 18
    // getting the ids of all processes
    int p1pid = getpid();
    int p4pid, p3pid;

    // msgBuff msg;
    msgrcv(mqId, &msg, sizeof(msg), 18, 0);
    sscanf(msg.msg, "%d", &p2pid);

    // msgBuff msg;
    msgrcv(mqId, &msg, sizeof(msg), 18, 0);
    sscanf(msg.msg, "%d", &p3pid);
    // msgBuff msg;

    msgrcv(mqId, &msg, sizeof(msg), 18, 0);
    sscanf(msg.msg, "%d", &p4pid);

    // todo - set the group ids
    setpgid(0, 0);
    setpgid(p2pid, getpid());
    setpgid(p3pid, getpid());
    setpgid(p4pid, getpid());
    
    cout<<getpgid(0)<<endl;
    cout<<getpgid(p2pid)<<endl;
    cout<<p2pid<<endl;
    cout<<getpgid(p3pid)<<endl;
    cout<<getpgid(p4pid)<<endl;

    msgctl(mqId, IPC_RMID, NULL);
}