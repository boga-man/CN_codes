#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <bits/stdc++.h>
using namespace std;

int main(){
    int x, y;

    // read x and y and add them
    cin >> x >> y;
    cout << x + y << endl;

    // read x and y and find the difference
    cin >> x >> y;
    cout << x - y << endl;

    // read x and y and find the product
    cin >> x >> y;
    cout << x * y << endl;

    return 0;
}
