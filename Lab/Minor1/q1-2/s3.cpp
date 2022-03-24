#include <bits/stdc++.h>
#include <signal.h>
#include <unistd.h>
using namespace std;

void handler(int sig){

    cout<<"From the service3 - HELLO!\n";
}

int main(){
    cout<<"Entered the service 1\n";
    signal(SIGUSR1, handler);
    while(1){
        pause();
    }
    return 0;
}