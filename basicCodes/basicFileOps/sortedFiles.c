#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

char *my_itoa(int num, char *str)
{
    if(str == NULL)
    {
        return NULL;
    }
    sprintf(str, "%d", num);
    return str;
}

int getNumbers(int fd, int arr[]){
    char buf[100];
    int indexCurrentSymbol = 0;
    int read_bytes;
    int file_read=0;

    buf[0] = '\0';
    char buff = '\0';
    while (file_read == 0) {
        read_bytes = read(fd, &buf[indexCurrentSymbol], 1);
        if (read_bytes == -1) 
           perror("read while: -1");
        if (read_bytes == 0) 
            file_read = 1;

        indexCurrentSymbol++;
    }   

    int j = 0;
    int fileSize = indexCurrentSymbol-1;
    for(int i = 0; i<indexCurrentSymbol; i++){
        int num = 0;
        while(i<indexCurrentSymbol-1 && buf[i]!=' '){
            num = num*10 + (int)buf[i] - (int)'0';
            i++;
        }
        arr[j++] = num;
    }

    return j;   
}

int main(){
    int fd1 = open("1f1.txt", O_RDWR|O_CREAT, 0777);
    int fd2 = open("1f2.txt", O_RDWR|O_CREAT, 0777);
    int resFile = open("1f3.txt", O_RDWR|O_CREAT, 0777);

    if(fd1<0 || fd2<0){
        printf("Input file not found");
        exit(1);
    }
    int arr1[100], arr2[100];
    int s1 = getNumbers(fd1, arr1);
    int s2 = getNumbers(fd2, arr2);

    char num[10];
    int i = 0, j = 0;

    while(i<s1 && j<s2){
        if(arr1[i]<arr2[j]){
            my_itoa(arr1[i], num);
            write(resFile, num, strlen(num));
            i++;
        }else{
            my_itoa(arr2[j], num);
            write(resFile, num, strlen(num));
            j++;
        }
        write(resFile, " ", 1);
    }

    while(i<s1){
        my_itoa(arr1[i], num);
        write(resFile, num, strlen(num));
        i++;
    }

    while(j<s2){
        my_itoa(arr2[j], num);
        write(resFile, num, strlen(num));
        j++;
    }


    close(fd1);
    close(fd2);
    close(resFile);

    return 0;
}


