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

pthread_t reader_id, writer_id;
int file_fd;

void *writer_thread(void *arg);
void *reader_thread(void *arg);
string file_name = "demo";

int main(){
    system("g++ p2.cpp -o p2");
    system("g++ p3.cpp -o p3");   
    // popen p2 process
    FILE *fd = popen("./p2", "w");
    file_fd = fileno(fd);
    pthread_create(&writer_id, NULL, writer_thread, NULL);
    pthread_create(&reader_id, NULL, reader_thread, NULL);
    pthread_join(reader_id, NULL);
    pthread_join(writer_id, NULL);
    system("rm ./demo ./p2 ./p3 ./a.out");
    return 0;
}

void *writer_thread(void *arg){
    // process p2 file
    cout<<"Writer thread initiated\n";
    char buf[100];
    // write to fifo file
    
    while(1){
        cin.getline(buf, 100);
        write(file_fd, buf, strlen(buf)+1);
        if(strcmp(buf, "exit") == 0){
            cout<<"Writer thread exiting\n";
            break;
        }
    }
}

void *reader_thread(void *arg){
    // fifo file
    cout<<"Reader thread initiated\n";
    // fifo file
    int fifo_fd;
    // open fifo file
    if((fifo_fd = mkfifo(file_name.c_str(), 0777)) == -1){
        perror("mkfifo");
        exit(1);
    }
    fifo_fd = open(file_name.c_str(), O_RDONLY);
    char buf[100];
    // read from fifo file
    while(1){
        read(fifo_fd, buf, 100);
        if(strcmp(buf, "exit") == 0){
            cout<<"Reader thread exiting\n";
            close(fifo_fd);
            break;
        }
        cout << "\nReceived: " << buf << endl;
    }
}
