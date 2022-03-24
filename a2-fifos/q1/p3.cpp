#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
using namespace std;

string file_name = "demo";

int main()
{
    cout << "Starting p3\n";
    mkfifo(file_name.c_str(), 0777);
    while (1)
    {
        char buff[100];
        int fifo_fd = open(file_name.c_str(), O_WRONLY);
        read(0, buff, 100);
        if (strcmp(buff, "exit") == 0)
        {
            write(fifo_fd, buff, strlen(buff) + 1);
            cout << "p3 exiting\n";
            break;
        }
        string received(buff);
        received = "Received from p1: " + received + "\n";
        cout << received;
        for (int i = 0; buff[i] != '\0'; i++)
        {
            if (buff[i] >= 'a' && buff[i] <= 'z')
            {
                buff[i] += ('A' - 'a');
            }
        }
        string msg(buff);
        msg = msg + " FROM P3\n";
        cout << "Written to fifo\n";
        write(fifo_fd, msg.c_str(), msg.length() + 1);
    }
}
