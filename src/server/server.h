#pragma once

#include "../client/client.h"
#include "../ioring/ioring.h"
#include <iostream>
#include <vector>
#include <string>

class Server{
public:
    Server(uint16_t port, int queueDepth, ioring* ioringptr, uint clientMaxBufferSize, unsigned long long clientWaitSec, std::string* messageForClient);

    /*
    При обновлении сервер асинхронно ожидает подключения клиентов, если подключается - создает экземляр с client.h
    */
    bool update();
    ~Server();
private:
    std::vector<Client*> mainClients;
    ioring::ClientData mainClientData;
    ioring* mainIoringptr;
    std::string* mainMessageForClient;
    float mainClientWaitSec;
    int mainClientMaxBuffersize;
};