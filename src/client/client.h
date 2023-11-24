#pragma once

#include "../ioring/ioring.h"
#include <liburing.h>
#include <vector>
#include <string>

class Client{
public:
    Client(int clientfd, ioring* ioringptr, uint clientMaxBufferSize, unsigned long long clientWaitSec, std::string* messageForClient);
    bool update();
    ~Client();
private:

    enum class CurrentState{
        waitMessage,
        WaitTimer
    };
    CurrentState mainState;
    ioring::ClientData* mainClientData;
    int mainclientfd;
    int mainClientMaxBufferSize;
    unsigned long long mainClientWaitSec;
    ioring* mainioringptr;
    std::string* mainMessageForClient;
};
