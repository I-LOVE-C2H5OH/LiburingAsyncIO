#include "ioring.h"
#include <iostream>
#include <fcntl.h>

ioring::ioring(int queue_depth, char* filename) {
    if (io_uring_queue_init(queue_depth, &mainRing, 0) != 0) {
        throw std::runtime_error("Unable to initialize io_uring");
    }
    mainfd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
}

void ioring::writeFile(char* buffer, uint32_t bufferSize)
{
    mainSQE = io_uring_get_sqe(&mainRing);
    io_uring_prep_write(mainSQE, mainfd, buffer, bufferSize, 0);
    io_uring_sqe_set_data(mainSQE, nullptr);
    io_uring_submit(&mainRing);
}

void ioring::addInQueueAccept(ClientData* clientData)
{
    mainSQE = io_uring_get_sqe(&mainRing);
    io_uring_prep_accept(mainSQE, clientData->fd, NULL, NULL, 0);
    io_uring_sqe_set_data(mainSQE, (void*)clientData);
    io_uring_submit(&mainRing);
}

void ioring::addInQueueRead(ClientData* clientData){
    mainSQE = io_uring_get_sqe(&mainRing);
    io_uring_prep_read(mainSQE, clientData->fd, clientData->buffer, clientData->buffersize, 0);
    io_uring_sqe_set_data(mainSQE, (void*)clientData);
    io_uring_submit(&mainRing);
}

void ioring::addInQueueSend(ClientData* clientData, char* buffer, int buffersize){
    mainSQE = io_uring_get_sqe(&mainRing);
    io_uring_prep_send(mainSQE, clientData->fd, buffer, buffersize, 0);
    io_uring_sqe_set_data(mainSQE, nullptr);
    io_uring_submit(&mainRing);
}

void ioring::addInQueueWait(ClientData* clientData, long long waitDelay){
    mainSQE = io_uring_get_sqe(&mainRing);
    struct __kernel_timespec delay = {
        .tv_sec = waitDelay,
        .tv_nsec = 0
    };
    io_uring_prep_timeout(mainSQE, &delay, 0, 0);
    io_uring_sqe_set_data(mainSQE, clientData);
    io_uring_submit(&mainRing);
}

int ioring::isMyEvent(ClientData* clientData)
{
    io_uring_wait_cqe(&mainRing, &mainCQE);
    if(!mainCQE)
    {
        throw std::runtime_error("Error in waiting CQE\n");
    }
    ClientData* CQEUserData = (ClientData*)io_uring_cqe_get_data(mainCQE);
    if(CQEUserData == nullptr)
    {
        io_uring_cqe_seen(&mainRing, mainCQE);
        return -1;
    }
    else if(clientData->uniqueID == CQEUserData->uniqueID)
    {
        int res = mainCQE->res;
        io_uring_cqe_seen(&mainRing, mainCQE);
        return res;
    }
    return -1;

}

ioring::~ioring()
{
    close (mainfd);
    io_uring_queue_exit(&mainRing);
}
