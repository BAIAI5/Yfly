#ifndef PINGPONG_SERVER_H
#define PINGPONG_SERVER_H

#include <map>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

class Server
{
public:
    ~Server()
    {
        free(ClientSock);
    }
    void init(int port);
    void connect();
private:
    int ServerSock{}, EpollFd{};
    int ConnectionCnt{};
    int *ClientSock = NULL;
    const int MaxConnection = 1000;
    const int EpollSize = 2000;
    struct epoll_event *EpollEvents;
    std::map<int, int> Time2Client;
};


#endif // PINGPONG_SERVER_H