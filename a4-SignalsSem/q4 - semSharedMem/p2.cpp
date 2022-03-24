#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
using namespace std;


key_t shmkeyx = ftok("./shmx_file", 65);
key_t shmkeyy = ftok("./shmy_file", 65);

const char *semx = "s1";
const char *semy = "s2";

int main(){
    int shmidx = shmget(shmkeyx, 16, 0666|IPC_CREAT);
    int shmidy = shmget(shmkeyy, 16, 0666|IPC_CREAT);

    int *x = (int *)shmat(shmidx, 0, 0);
    int *y = (int *)shmat(shmidy, 0, 0);
    
    sem_t *S1 = sem_open(semx, O_EXCL);
    sem_t *S2 = sem_open(semy, O_EXCL);

    cout<<"I am waiting for S1\n";
    sem_wait(S1);
    int read_x = *x;
    *y = read_x + 1;
    cout<<"Enter any character to signal S2\n";
    char c; cin>>c;
    sem_post(S2);

    shmdt(x);
    shmdt(y);

    sem_unlink(semx);
    sem_unlink(semy);
    return 0;
}