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
    string message = "This is child Process\n";
    char mess[message.length()];
    strcpy(mess, message.c_str());
    int std_in = argv[1][0];
    int std_out = argv[1][1];
    // int readFD = 0, writeFD = 1;
    int readFD = argv[1][2];
    int writeFD = argv[1][3];
    write(std_out, mess, strlen(mess));
    while (true)
    {
        char buff[100];
        memset(buff, 0, 100);
        int readbytes = read(readFD, buff, 100);
        if (readbytes == 0)
            exit(0);
        else
        {
            string message = "message read in child process\n";
            message += string(buff);
            char mess[message.length()];
            strcpy(mess, message.c_str());
            write(std_out, mess, strlen(mess));
        }
        string message = "enter the message to be written by child process: ";
        char mess[message.length()];
        strcpy(mess, message.c_str());
        write(std_out, mess, strlen(mess));
        buff[100];
        int inp = read(std_in, buff, 100);
        if (inp == 0 || (inp == 1 && buff[0] == '\n'))
            exit(0);
        write(writeFD, buff, inp);
    }
}