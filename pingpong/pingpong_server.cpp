#include "pingpong_server.h"

#include <cstdio>

void Server::init(int port)
{
    // init server socket
    struct sockaddr_in ServerAddr;
    if ((ServerSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        ErrorHandling("socket() error");
    
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    ServerAddr.sin_port = htons(port);

    if (bind(ServerSock, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr)) < 0)
        ErrorHandling("bind() error");
    
    if (listen(ServerSock, 1024) < 0)
        ErrorHandling("listen() error");

    ClientSock = (int *)malloc(sizeof(int) * MaxConnection);

    // init epoll fd
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = ServerSock;
    EpollFd = epoll_create(EpollSize);
    EpollEvents = (struct epoll_event *)malloc(sizeof(epoll_event) * EpollSize);
    epoll_ctl(EpollFd, EPOLL_CTL_ADD, ServerSock, &event);
}

void Server::connect()
{
    
}



void ErrorHandling(const char *ErrorMsg)
{
    fputs(ErrorMsg, stderr);
    fputc('\n', stderr);
    exit(1);
}