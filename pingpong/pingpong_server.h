#ifndef PINGPONG_SERVER_H
#define PINGPONG_SERVER_H

#include <server_epoll.h>

class Server
{
public:
    Server(int port);
    void run();

private:
    int server_socket;
    ServerEpoll server_epoll;
};

#endif // PINGPONG_SERVER_H