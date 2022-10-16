#ifndef PINGPONG_SERVER_H
#define PINGPONG_SERVER_H

#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/socket.h>
#include <map>

class Server
{
public:
    void init();
    void run();
private:
    int ServerSock, EpollFd;
    int ConnectionCnt{};
    std::map<int, int> Time2Client;
};


#endif // PINGPONG_SERVER_H