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
}

bool Client::update()
{
    int result = mainioringptr->isMyEvent(mainClientData);
    
    if(result == 0 && mainState == CurrentState::waitMessage)
    {
        return false;
    }
    else if(result > 0 && mainState == CurrentState::waitMessage)
    {
        mainioringptr->writeFile(mainClientData->buffer, result);
        mainState = CurrentState::WaitTimer;
        mainioringptr->addInQueueWait(mainClientData, mainClientWaitSec);
    }
    else if(mainState == CurrentState::WaitTimer && result != -1)
    {
        char* buffer = new char[mainMessageForClient->length() + 1];
        memset(buffer, mainMessageForClient->length(), 0);
        memcpy(buffer, mainMessageForClient->c_str(), mainMessageForClient->length());
        mainioringptr->addInQueueSend(mainClientData, buffer, mainMessageForClient->length() + 1);
        mainState = CurrentState::waitMessage;
        mainioringptr->addInQueueRead(mainClientData);
    }
    return true;
}

Client::~Client()
{
    free(mainClientData->buffer);
    delete mainClientData;
}
