#pragma once

#include <liburing.h>

/*
  Основной класс ассинхронного кольца ввода-вывода
  Должен быть мэмбэром в классах.
*/
class ioring {
public:

  enum class EventType{
    NewMessage,
    Wait,
    Write,
    Send
  };

  struct ClientData{
    uint32_t buffersize;
    int uniqueID;
    int fd;
    EventType type;
    char* buffer;
    };

  ioring(int queue_depth, char* filename);
  char* readFile(char* filename);
  void writeFile(char* buffer, uint32_t bufferSize);
  void addInQueueAccept(ClientData* clientData);
  void addInQueueRead(ClientData* clientData);
  void addInQueueSend(ClientData* clientData, char* buffer, int buffersize);
  
  /*
  Проверяет, относитcя ли текущий event к данному объекту.
  */
  int isMyEvent(ClientData* clientData);
  ~ioring();
private:
  struct io_uring_sqe* mainSQE;
  struct io_uring_cqe* mainCQE;
  struct io_uring mainRing;
  int mainfd;
};