#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
using namespace std;

const char *s34 = "/s34";
const char *s41 = "/s41";

int main(){
    sem_t *S41 = sem_open(s41, O_EXCL);
    sem_t *S34 = sem_open(s34, O_EXCL);
    cout<<"I am P4, I am waiting for semaphore S34\n";
    sem_wait(S34);
    cout<<"I got semaphore signalling from P3\n";
    cout<<"enter any character to sem-signal(S41)\n";
    char c; cin>>c;
    cout<<"I am signalling semaphore signal of S41\n";
    sem_post(S41);
    return 0;
}