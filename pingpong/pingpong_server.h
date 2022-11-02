#ifndef PINGPONG_SERVER_H
#define PINGPONG_SERVER_H

#include "server_epoll.h"
#include <vector>

class Server
{
public:
    Server(int port, int num);
    void run(int index);
    void Accept();
private:
    int server_socket;
    int connection_cnt;
    int thread_num;
    std::vector<ServerEpoll> server_epolls;
};

#endif // PINGPONG_SERVER_H