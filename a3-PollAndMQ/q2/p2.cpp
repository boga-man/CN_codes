#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
using namespace std;

int main(){
    mkfifo("fifo1", 0777);
    cout<<"Opening fifo file\n";
    int fd = open("fifo1", O_WRONLY);
    cout<<fd<<endl;
    cout<<"Fifo file opening status: "<<fd<<endl;
    cout<<"Writing to the fifo file\n";
    write(fd, "From p2\n", strlen("from p2\n"));
    cout<<"Exiting the process\n";
    close(fd);
    return 0;
}
