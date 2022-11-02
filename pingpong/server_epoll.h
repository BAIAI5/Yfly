#ifndef EPOLL_H
#define EPOLL_H

#include <vector>
#include <sys/epoll.h>
#include "server_handler.h"

class ServerEpoll
{
public:
    ServerEpoll();
    ~ServerEpoll();
    void EventRegister(int event_fd);
    int EventWait();
    void EventRemove(int event_fd);

private:
    int epoll_fd;
    std::vector<struct epoll_event> events;
    static const int max_event;
    ServerHandler server_handler;
};

#endif // EPOLL_H