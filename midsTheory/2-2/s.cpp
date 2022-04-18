#include "myHeader.h"

vector<int> nsfds;
vector<ServerDataString> clientsData;
vector<pthread_t> tids;
int nassfd;
ServerDataString AS_ADDR_STR;

bool isMaint = false;

// Initialise maintenance mode
// i.e send all the clients information to AS so that it can serve them
// here we should be repeatedly sending any new client connections
void InitMaintananceMode(bool isMaint){
    // send the stored clients data to AS and then clear the data in the end
    cout << "Going to Maintanance Mode...\n";
    Writefd(nassfd, "1");
    for (int i = 0; i < nsfds.size(); i++)
    {
        string s = clientsData[i].serverData;
        Writefd(nassfd, s);
        Writefd(nsfds[i], AS_ADDR_STR.serverData);
        pthread_cancel(tids[i]);
        close(nsfds[i]);
    }
    Writefd(nassfd, "end");

    // clearing the clients data in the process
    nsfds.clear();
    clientsData.clear();
    tids.clear();
}

void *ServerThread(void *arg){
    int sfd = *(int *)arg;
    // accept any incoming connection, store its information and create new thread for the clientService
    while (1)
    {
        struct sockaddr_in caddr;
        int addrlen = sizeof(caddr);
        int nsfd;
        // accept the incoming connection and get nsfd
        if ((nsfd = accept(sfd, (struct sockaddr *)&caddr, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        cout << "New client connected in S server..." << endl;


        
        // storing the client nsfd to notify it maintanance time
        nsfds.push_back(nsfd);
        ServerData clientData = getPeerName(nsfd);
        ServerDataString clientString = getServerDataString(clientData); 

        // using new thread to service the client
        pthread_t client_thread;
        // if the S is not in maintanance mode, then make new thread to serve the client
        if(!isMaint){
            clientsData.push_back(clientString);
            pthread_create(&client_thread, NULL, ClientSerivceHandler, (void *)&nsfd);
            tids.push_back(client_thread);
        }
        // if the S is in maintanance mode, then send the AS address to the client, and client address to the AS
        else{
            // sending the AS details to the client
            send(nsfd, AS_ADDR_STR.serverData.c_str(), AS_ADDR_STR.serverData.size(), 0);
            // close the connection with the client
            close(nsfd);
        }
    }
    return NULL;
}

int main(){
    // initialise the AS_ADDR_STR
    AS_ADDR_STR.serverData = AS_ADDR;

    // server opens its port
    int sfd = InitTcpServer(3000);

    // server opens a port to communicate with the Alternate Server
    int assfd = InitTcpServer(8000);

    // first connect to the AS and only then listen to the clients
    listen(assfd, SOMAXCONN);

    // accept the Alternate server connection
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    cout << "Waiting for AS to connect...\n";
    if ((nassfd = accept(assfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // after AS connects, now we can listen to the sfd for clients
    if (listen(sfd, SOMAXCONN) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    pthread_t id;
    // S serves the clients in different thread while it communicates with AS continuously to update its status to AS
    pthread_create(&id, NULL, ServerThread, &sfd);

    // update the status of S to AS via socket messages
    while(1){
        cout << "Mode: " << (!isMaint ? "Working" : "Maintenance") << " Toggle? y/n";
        char ch;
        cin >> ch;
        isMaint = ch == 'y' ? !isMaint : isMaint;
        if(isMaint == 1)
            InitMaintananceMode(isMaint);
        else
            Writefd(nassfd, "1");
        sleep(10);
    }
}