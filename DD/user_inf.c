#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(){

    // opening the driver
    int fd = open("/dev/cap_driver",O_RDWR);

    // interacting with the driver
    while(1){
        printf("Enter 1 to write into kernel or 2 to read from kernel\n");
        int c;
        scanf("%d",&c);
        if(c==1){
            printf("Enter the content to be written...\n");
            char buf[1024];
            scanf("%s",buf);
            write(fd,buf,sizeof(buf));
        }
        else if(c==2){
            char buf[300];
            read(fd,buf,sizeof(buf));
            printf("Content read from kernel is: %s\n",buf);
        }
        else{
            printf("Enter correct value\n");
        }
    }
    return 0;
}