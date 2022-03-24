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

const char *s23 = "/s23";
const char *s34 = "/s34";

int main(){
    sem_t *S23 = sem_open(s23, O_EXCL);
    sem_t *S34 = sem_open(s34, O_CREAT|O_EXCL, 0666, 0);
    cout<<"I am P3, I am waiting for semaphore S23\n";
    sem_wait(S23);
    cout<<"I got semaphore signalling from P2\n";
    cout<<"enter any character to sem-signal(S34)\n";
    char c; cin>>c;
    cout<<"I am signalling semaphore signal of S34\n";
    S34 = sem_open(s34, O_EXCL);
    sem_post(S34);

    sem_unlink(s34);
    return 0;
}