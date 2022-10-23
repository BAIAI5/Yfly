#ifndef EPOLL_H
#define EPOLL_H

#include <vector>
#include "server_handler.h"

class ServerEpoll
{
public:
    ServerEpoll();
    ~ServerEpoll();
    void EventRegister(int event_fd);
    void EventWait();
    void EventRemove(int event_fd);

private:
    int epoll_fd;
    int first_fd;
    std::vector<struct epoll_event> events;
    static const int max_event;
    ServerHandler event_handler;
};

#endif // EPOLL_H