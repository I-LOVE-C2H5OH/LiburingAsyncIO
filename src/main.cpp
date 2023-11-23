#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <liburing.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "ioring/ioring.h"
#include "server/server.h"

#define QUEUE_DEPTH 1024
#define BUFFER_SIZE 128
#define clientWaitAcceptedSend 3

int main(int argc, char** argv) {
    if(argc != 2)
    {
        printf("Clease use this witch\n./liburing_tcp <port>\n");
        return -1;
    }
    int port = std::stoi(argv[1]);
    std::string message = "ACCEPTED\n";
    std::string filename = std::to_string(port) + ".txt";
    ioring* ioring_ptr = new ioring(QUEUE_DEPTH, (char*)filename.c_str());
    Server* server_ptr = new Server(port, QUEUE_DEPTH, ioring_ptr, BUFFER_SIZE, clientWaitAcceptedSend, &message);

    while (1)
    {
        bool res = server_ptr->update();
        if(!res)
        {break;}
    }
    delete server_ptr;
    delete ioring_ptr;
    return 0;
}
