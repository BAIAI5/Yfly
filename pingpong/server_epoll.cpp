#include "server_epoll.h"

#include <unistd.h>
#include <sys/epoll.h>
#include <assert.h>

const int ServerEpoll::max_event = 2010;
ServerEpoll::ServerEpoll()
: epoll_fd(-1)
{
    epoll_fd = epoll_create(max_event);
    events.resize(max_event);
}

ServerEpoll::~ServerEpoll()
{
    if (epoll_fd != -1)
        close(epoll_fd);
}

void ServerEpoll::EventRegister(int event_fd)
{
    int timer_fd = server_handler.HandleConnect(event_fd);
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = timer_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, timer_fd, &event);
}

void ServerEpoll::EventRemove(int event_fd)
{
    struct epoll_event event;
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event);
}

int ServerEpoll::EventWait()
{
    int event_num = epoll_wait(epoll_fd, &events[0], max_event, -1);
    assert(event_num != -1);
    for (int i = 0; i < event_num; ++i)
    {
        if (server_handler.HandleTime(events[i].data.fd) < 0)
            return -1;
    }
    return 1;
}