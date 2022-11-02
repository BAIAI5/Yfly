#include "pingpong_server.h"

#include <stdio.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>

void ErrorHandling(const char *ErrorMsg)
{
    fputs(ErrorMsg, stderr);
    fputc('\n', stderr);
    exit(1);
}

Server::Server(int port, int num)
: connection_cnt(0), thread_num(num)
{
    struct sockaddr_in server_addr;
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        ErrorHandling("socket() error");
    
    server_addr.sin_family = AF_INET;
    //server_addr.sin_addr.s_addr = inet_addr("192.168.136.133");
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        ErrorHandling("bind() error");
    
    if (listen(server_socket, 1024) < 0)
        ErrorHandling("listen() error");

    server_epolls = std::vector<ServerEpoll>(thread_num);

    printf("port: %d\n", port);
}

void Server::run(int index)
{
    ServerEpoll &server_epoll = server_epolls[index];
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

        //printf("accept %d\n", client_socket);
        server_epolls[connection_cnt % thread_num].EventRegister(client_socket);
	    connection_cnt++;
        if (connection_cnt % 1000 == 0)
            printf("connection_cnt: %d\n", connection_cnt);
        if (connection_cnt >= 200000)
        {
            puts("stop accept");
            break;
        }
    }
}
