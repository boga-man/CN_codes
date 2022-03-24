#include "fd_passing.h"
using namespace std;

int main(){
    // initiating a raw socket
    int rsfd = RawSocketInit(adProtoNum);

    // initiating the platforms addresses (this system only)
    struct sockaddr_in client;
	client.sin_family=AF_INET;
	client.sin_addr.s_addr=inet_addr("127.0.0.1");

    // send messages from this raw socket at regular intervals
    cout<<"Initiating sending ads...\n";
    while(1){
        sleep(5);
        string ad = "This is an ad from Ad server\n";
        sendto(rsfd, ad.c_str(), ad.size(), 0, (struct sockaddr*)&client, sizeof(client));
        
    }
}