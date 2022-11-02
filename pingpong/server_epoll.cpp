#include "server_epoll.h"

#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <iostream>

const int ServerEpoll::max_event = 50000;
ServerEpoll::ServerEpoll()
: epoll_fd(-1)
{
    epoll_fd = epoll_create(max_event);
    events.resize(max_event);
    //server_handler = new ServerHandler;
}

ServerEpoll::~ServerEpoll()
{
    if (epoll_fd != -1)
        close(epoll_fd);
}

void ServerEpoll::EventRegister(int event_fd)
{
    struct epoll_event event;

    int timer_fd = server_handler.HandleConnect(event_fd);
    event.events = EPOLLIN;
    event.data.fd = timer_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, timer_fd, &event);

    event.events = EPOLLIN;
    event.data.fd = event_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_fd, &event);
}

void ServerEpoll::EventRemove(int event_fd)
{
    struct epoll_event event;
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event);
}

int ServerEpoll::EventWait()
{
    int event_num = epoll_wait(epoll_fd, &events[0], max_event, -1);
    if (event_num == -1)
    {
        if (errno != EINTR)
        {
            return -1;
        }
        
        return 1;
    }
    //assert(event_num != -1);

    int fd;
    for (int i = 0; i < event_num; ++i)
    {
        fd = events[i].data.fd;
        
        if (server_handler.FdType(fd) == TIMER_FD)
        {
            server_handler.HandleTime(fd);
        }
        else if (server_handler.FdType(fd) == CLIENT_SOCKET)
        {
            server_handler.HandleRead(fd);
        }
        else
        {
            printf("error event fd type %d\n", fd);
            return -1;
        }
    }

    return 1;
}