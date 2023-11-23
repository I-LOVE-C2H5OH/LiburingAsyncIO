#include "server.h"
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>


Server::Server(uint16_t port, int queueDepth, ioring* ioringptr, uint clientMaxBufferSize, unsigned long long clientWaitSec, std::string* messageForClient)
: mainIoringptr(ioringptr), mainClientMaxBuffersize(clientMaxBufferSize), mainClientWaitSec(clientWaitSec), mainMessageForClient(messageForClient){
    struct sockaddr_in server_addr;

    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        return;
    }

    if (listen(server_fd, queueDepth) < 0) {
        perror("listen");
        return;
    }
    mainClientData.fd = server_fd;
    mainClientData.uniqueID = server_fd;
    ioringptr->addInQueueAccept(&mainClientData);
}

bool Server::update()
{
    int result = mainIoringptr->isMyEvent(&mainClientData);
    for (uint i = 0; i < mainClients.size(); i++)
    {
        bool result = mainClients[i]->update();
        if(!result)
        {
            printf("ClientDisconnected\n");
            delete mainClients[i];
            mainClients.erase(mainClients.begin() + i);
            if(mainClients.size() == 0)
            {
                return false;
            }
        }
    }
    //Подключился новый клиент
    if(result > 0)
    {
        mainIoringptr->addInQueueAccept(&mainClientData);
        Client* newClient = new Client(result, mainIoringptr, mainClientMaxBuffersize, mainClientWaitSec, mainMessageForClient);
        mainClients.push_back(newClient);
    }
    return true;
}

Server::~Server(){
    close(mainClientData.fd);
}
