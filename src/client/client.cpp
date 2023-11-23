#include "client.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Client::Client(int clientfd, ioring* ioringptr, uint clientMaxBufferSize, unsigned long long clientWaitSec, std::string* messageForClient) 
: mainioringptr(ioringptr), mainClientWaitSec(clientWaitSec), mainMessageForClient(messageForClient), mainclientfd(clientfd)
{
    mainClientData = new ioring::ClientData();
    mainClientData->buffersize = clientMaxBufferSize;
    mainClientData->buffer = (char*)malloc(clientMaxBufferSize);
    mainClientData->fd = clientfd;
    mainClientData->uniqueID = clientfd;
    mainClientData->type=ioring::EventType::NewMessage;
    mainioringptr->addInQueueRead(mainClientData);
    printf("ClientHello!!!\n");
}

bool Client::update()
{
    if(mainState == CurrentState::WaitTimer)
    {
        clock_t currentClocks = clock();
        if(((currentClocks - mainTimer)/CLOCKS_PER_SEC) > mainClientWaitSec)
        {
            char* buffer = new char[mainMessageForClient->length() + 1];
            memset(buffer, mainMessageForClient->length(), 0);
            memcpy(buffer, mainMessageForClient->c_str(), mainMessageForClient->length());
            mainioringptr->addInQueueSend(mainClientData, buffer, mainMessageForClient->length() + 1);
            mainState = CurrentState::waitMessage;
        }
    }
    int result = mainioringptr->isMyEvent(mainClientData);
    if(result == 0)
    {
        printf("client disconnected\n");
        return false;
    }
    if(result > 0)
    {
        printf("client send: %s\n", mainClientData->buffer);
        mainioringptr->writeFile(mainClientData->buffer, result);
        mainioringptr->addInQueueRead(mainClientData);
        mainState = CurrentState::WaitTimer;
        mainTimer = clock();

   }

    return true;
}

Client::~Client()
{
    
}