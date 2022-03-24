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

const char *s12 = "/s12";
const char *s23 = "/s23";

int main(){
    sem_t *S12 = sem_open(s12, O_EXCL);
    sem_t *S23 = sem_open(s23, O_CREAT|O_EXCL, 0666, 0);
    cout<<"I am P2, I am waiting for semaphore S12\n";
    sem_wait(S12);
    cout<<"I got semaphore signalling from P1\n";
    cout<<"enter any character to sem-signal(S23)\n";
    char c; cin>>c;
    cout<<"I am signalling semaphore signal of S23\n";
    S23 = sem_open(s23, O_EXCL);
    sem_post(S23);

    sem_unlink(s23);
    return 0;
}