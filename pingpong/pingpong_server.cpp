#include "pingpong_server.h"

#include <cstdio>
#include <arpa/inet.h>

void ErrorHandling(const char *ErrorMsg)
{
    fputs(ErrorMsg, stderr);
    fputc('\n', stderr);
    exit(1);
}

Server::Server(int port)
{
    // init server socket
    struct sockaddr_in server_addr;
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        ErrorHandling("socket() error");
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        ErrorHandling("bind() error");
    
    if (listen(server_socket, 1024) < 0)
        ErrorHandling("listen() error");

    server_epoll.EventRegister(server_socket);

    // init epoll fd
    // struct epoll_event event;
    // event.events = EPOLLIN;
    // event.data.fd = ServerSock;
    // EpollFd = epoll_create(EpollSize);
    // EpollEvents = (struct epoll_event *)malloc(sizeof(epoll_event) * EpollSize);
    // epoll_ctl(EpollFd, EPOLL_CTL_ADD, ServerSock, &event);
}

void Server::run()
{
    while (1)
    {
        server_epoll.EventWait();
    }
}