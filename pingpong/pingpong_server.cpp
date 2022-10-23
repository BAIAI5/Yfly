#include "pingpong_server.h"

#include <stdio.h>
#include <arpa/inet.h>
#include <assert.h>

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
}

void Server::run()
{
    while (1)
    {
        server_epoll.EventWait();
    }
}

void Server::Accept()
{
    while (1)
    {
        int client_socket = accept(server_socket, NULL, NULL);
        assert(client_socket != -1);

        printf("accept %d\n", client_socket);
        server_epoll.EventRegister(client_socket);
    }
}