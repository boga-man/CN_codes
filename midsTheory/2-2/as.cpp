#include "myHeader.h"

vector<int> sfds;
vector<ServerData> clientsData;
vector<int> nsfds;
vector<int> tids;

ServerDataString S_ADDR_STR;

void Poll(vector<int> fds)
{
    int pSize = fds.size();
    struct pollfd pfds[pSize];
    // initalising the pfds
    for (int i = 0; i < pSize; i++)
    {
        struct pollfd pollfd;
        pfds[i].fd = fds[i];
        pfds[i].events = POLLIN | POLLPRI;
        pfds[i].revents = 0;
    }
    while (1)
    {
        int pollResult = poll(pfds, pSize, -1);
        if (pollResult > 0)
        {
            for (int i = 0; i < pSize; i++)
            {
                if (pfds[i].revents & POLLIN)
                {
                    pfds[i].revents = 0;

                    // i==0 means message from S
                    if (i == 0)
                    {
                        // on port 3000
                        // reading messages
                        int nsfd = pfds[0].fd;
                        string s = Readfd(nsfd);
                        if (s.size() == 0)
                            continue;

                        if (s == "1")
                        {
                            cout << "S is in maintainance mode...\n";
                            cout << "Getting the clients data from S\n";
                            // requesting for usage of alternate server
                            // rec all the valid pids
                            while (true)
                            {
                                string s = Readfd(nsfd);
                                if (s.size() == 0 || s == "end")
                                    break;
                                ServerData data = IntepretServerChangeMessage(s);
                                clientsData.push_back(data);
                            }
                            int n = clientsData.size();
                            printf("Rec %d pids to service...\n", n);
                        }
                        else
                        {
                            cout << "S is restoring its status to working mode...\n";
                            // telling to stop providing the service
                            for (int i = 0; i < nsfds.size(); i++)
                            {
                                Writefd(nsfds[i], S_ADDR_STR.serverData.c_str(), S_ADDR_STR.serverData.size(), 0);
                                pthread_cancel(tids[i]);
                            }
                            clientsData.clear();
                        }
                    }

                    // else some connection request to sfd
                    else
                    {
                        // on port 4000

                        struct sockaddr_in cliaddr;
                        int addrlen = sizeof(cliaddr);
                        int nsfd = accept(pfds[i].fd, NULL, NULL);
                        if (nsfd < 0)
                        {
                            perror("accept");
                            exit(EXIT_FAILURE);
                        }

                        // TODO: change this to getPeerName
                        cout << "New client connected in AS server..." << endl;
                        int pid = GetPID(nsfd);
                        cout << "Client says its PID is: " << pid << endl;

                        // alternate server
                        // allow only certain pids
                        // TODO: change this to IfClientDataExists
                        if (IfPIDExists(pid))
                        {
                            printf("Allowing the client with pid: %d....\n", pid);
                            pthread_t client_thread;
                            pthread_create(&client_thread, NULL, ClientHandler, (void *)&nsfd);
                            nsfds.push_back(nsfd);
                            tids.push_back(client_thread);
                        }
                        else
                        {
                            cout << "Removing the client....\n";
                            close(nsfd);
                        }
                    }
                }
            }
        }
    }
}

int main(){
    // initialise S_ADDR_STR
    AS_ADDR_STR.serverData = S_ADDR;

    // first connect to the S
    int asfd = ConnectToTcpServer("127.0.0.1", 8000);
    sfds.push_back(asfd);

    // now create a TCP server
    int sfd = InitTcpServer(4000);

    listen(sfd, SOMAXCONN);
    sfds.push_back(sfd);
    // now repeatedly check for message from the S
    // if it is in maintanance mode, then start a server in new thread 
    // utilise Poll for I/O multiplexing
    Poll();
    
}