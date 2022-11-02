#include "pingpong_client.h"

#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

Client::Client(std::vector<std::string> &ip, int num, int connect_num)
: client_ip(ip), thread_num(num), max_connection(connect_num), connection_num(0)
{
    int epoll_fd;
    for (int i = 0; i < thread_num; i++)
    {
        epoll_fd = epoll_create(max_connection / thread_num + 1);
        epoll_fds.push_back(epoll_fd);
        printf("i = %d epoll_fd: %d\n", i, epoll_fd);
    }

    events.resize(thread_num);
    for (int i = 0; i < thread_num; i++)
    {
        events[i].resize(max_connection / thread_num + 1);
    }

    sockets.resize(thread_num);
}

Client::~Client()
{   
    for (size_t i = 0; i < epoll_fds.size(); i++)
        close(epoll_fds[i]);
    
    for (size_t i = 0; i < sockets.size(); i++)
    {
        for (size_t j = 0; j < sockets[i].size(); j++)
            close(sockets[i][j]);
    }
}

void Client::EventRegister(int epoll_fd, int event_fd)
{
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = event_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_fd, &event);
}

void Client::Connect(const char * server_ip, int server_port)
{
    int ip_connection = max_connection / client_ip.size();
    for (size_t i = 0; i < client_ip.size(); i++)
    {
        std::string &ip = client_ip[i];
        for (int j = 0; j < ip_connection; j++)
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
                printf("client ip:%s server ip: %s server port: %d\n", ip.c_str(), server_ip, server_port);
                printf("errno: %s\n", strerror(errno));
                return;
            }

            int index = connection_num % thread_num;
            sockets[index].push_back(sock);
            EventRegister(epoll_fds[index] , sock);

            connection_num++;

            if (connection_num % 1000 == 0)
            {
                printf("pingpong_client connect %d\n", sock);
                sleep(1);
            }
        }
    }
}

int Client::PingPong(int sock)
{
    //printf("connection %d start pingpong\n", sock);
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
    //printf("client recv msg: %s\n", buf);

    char send_msg[] = "pong";
    write(sock, send_msg, strlen(send_msg));
    //printf("client send msg: %s\n\n", send_msg);

    return 1;
}

void Client::Respond(int index)
{
    printf("client thread %d\n", index);
    int epoll_fd = epoll_fds[index];

    std::vector<struct epoll_event> &temp_event = events[index];
    while (1)
    {
        int event_num = epoll_wait(epoll_fd, &temp_event[0], max_connection / 4 + 1, -1);
        if (event_num < 0)
        {
            printf("thread %d epoll_fd %d event_num %d temp_eventsz %ld\n", index, epoll_fd, event_num, temp_event.size());
            printf("errno: %s\n", strerror(errno));
            return;
        }
        for (int i = 0; i < event_num; i++)
        {
            if (PingPong(temp_event[i].data.fd) < 0)
                return;
        }
    }
}
