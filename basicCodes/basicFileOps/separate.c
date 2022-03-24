#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
    int in = open("3f1.txt", O_RDONLY);
    int out = open("3f2.txt", O_RDWR|O_CREAT, 0777);

    if(in<0){
        printf("Input file not found");
        exit(1);
    }

    int fileRead = 0;
    char buff;
    char nums[100];
    int j = 0;
    while(fileRead == 0){
        int charsRead = read(in, &buff, 1);
        if(charsRead==0){
            break;
        }
        if(buff>='0' && buff<='9'){
            nums[j++] = buff;
        }else{
            write(out, &buff, 1);
        }
    }

    int p = lseek(out,1000,SEEK_SET);
    
    for(int i = 0; i<j; i++){
        write(out, &nums[i], 1);
    }

    return 0;
}