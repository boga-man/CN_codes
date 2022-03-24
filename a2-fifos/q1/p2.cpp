#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
using namespace std;

int main(){
    cout<<"Starting p2\n";
    char buff[100];
    FILE* fd = popen("./p3", "w");
    int file_fd = fileno(fd);
    dup2(file_fd, 1);
    while(1){
        // cin>>buff;
        // cout<<buff;
        read(0,buff,100);
        write(1,buff,strlen(buff)+1);
        if(strcmp("exit",buff)==0){
            cout<<"P2 exiting\n";
            break;
        }
    }
}
