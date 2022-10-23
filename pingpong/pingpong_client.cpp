#include "pingpong_client.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>


const int Client::max_connection = 2010;

Client::Client(std::vector<std::string> &ip)
: client_ip(ip), epoll_fd(-1)
{
    epoll_fd = epoll_create(max_connection);
    events.resize(max_connection);
}

Client::~Client()
{
    if (epoll_fd != -1)
        close(epoll_fd);
    
    for (int &sock : sockets)
        close(sock);
}

void Client::EventRegister(int event_fd)
{
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = event_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_fd, &event);
}

void Client::Connect(const char * server_ip, int server_port)
{
    for (size_t i = 0; i < client_ip.size(); ++i)
    {
        std::string &ip = client_ip[i];
        for (int j = 0; j < max_connection; ++j)
        {
            int sock; 
            if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                printf("socket() error\n");
                return;
            }

            struct sockaddr_in client_addr;
            client_addr.sin_family = AF_INET;
            client_addr.sin_addr.s_addr = inet_addr(ip.c_str());
            client_addr.sin_port = 0;
            if(bind(sock,  (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
            {
                printf("bind() error\n");
                return;
            }

            struct sockaddr_in server_addr;
            server_addr.sin_family = AF_INET;
            server_addr.sin_addr.s_addr = inet_addr(server_ip);
            server_addr.sin_port = htons(server_port); 
            if(connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
            {
                printf("connect() error\n");
                return;
            }

            sockets.push_back(sock);
            EventRegister(sock);

            if (sockets.size() % 500 == 0)
                sleep(1);
        }
    }
}

int Client::PingPong(int sock)
{
    printf("connection %d start pingpong\n", sock);
    int expect_len = 4;
    int recv_len = 0, recv_cnt;
    const int BUF_SIZE = 10;
    char buf[BUF_SIZE];
    while (recv_len < expect_len)
    {
        recv_cnt = read(sock, &buf[recv_len], BUF_SIZE);
        if (recv_cnt == -1)
        {
            puts("read() error");
            return -1;
        }
        recv_len += recv_cnt;
    }
    buf[recv_len] = '\0';
    printf("client recv msg: %s\n", buf);

    char send_msg[] = "pong";
    write(sock, send_msg, strlen(send_msg));
    printf("client send msg: %s\n\n", send_msg);

    return 1;
}

void Client::Respond()
{
    while (1)
    {
        int event_num = epoll_wait(epoll_fd, &events[0], max_connection, -1);
        for (int i = 0; i < event_num; ++i)
        {
            if (PingPong(events[i].data.fd) < 0)
                return;
        }
    }
}