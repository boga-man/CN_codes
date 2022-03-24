#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>



int main(){
    int in = open("f1.txt", O_RDONLY);
    int out = open("f2.txt", O_RDWR|O_CREAT, 0777);

    if(in<0){
        printf("Input file not found");
        exit(1);
    }

    int fileRead = 0;
    char buff;
    while(fileRead == 0){
        int charsRead = read(in, &buff, 1);
        if(charsRead == 0){
            break;
        }
        printf("%c", buff);
        if(buff>='a' && buff<='z'){
            buff = buff + 'A' - 'a';    
        }
        
    }

    return 0;
}