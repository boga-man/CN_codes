#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
using namespace std;

int main(int argc, char **argv)
{
    int childFD[2], grandFD[2];
    pipe(childFD); pipe(grandFD);
    int std_in = argv[1][0];
    int std_out = argv[1][1];
    // int readFD = 0, writeFD = 1;
    int readFD = argv[1][2];
    int writeFD = argv[1][3];
    int pid = fork();
    if(pid>0){
        close(childFD[0]);
        close(grandFD[1]);
        while (true)
        {
            char buff[100];
            memset(buff, 0, 100);
            int readbytes = read(readFD, buff, 100);
            if (readbytes == 0)
                exit(0);
            else
            {
                write(childFD[1], buff, strlen(buff));
                string message = "Received message from parent. Passing it to grand child\n";
                char mess[message.length()];
                strcpy(mess, message.c_str());
                write(std_out, mess, strlen(mess));
            }
            int inp = read(grandFD[0], buff, 100);
            string message = "Received message from grand child. Passing it to parent\n";
            char mess[message.length()];
            strcpy(mess, message.c_str());
            write(std_out, mess, strlen(mess));
            buff[100];
            if (inp == 0 || (inp == 1 && buff[0] == '\n'))
                exit(0);
            write(writeFD, buff, inp);
        }
    }else{
        close(grandFD[0]);
        close(childFD[1]);
        char args[5];
        memset(args, 0, 3);
        args[0] = std_in;
        args[1] = std_out;
        args[2] = childFD[0];
        args[3] = grandFD[1];
        char *argv[] = {"./p3", args, NULL};
        execv("./p3", argv);
    }
}