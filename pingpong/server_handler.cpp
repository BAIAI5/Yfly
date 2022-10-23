#include "server_handler.h"

#include <sys/socket.h>
#include <sys/timerfd.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

static int cnt_time, cnt_connet;

ServerHandler::~ServerHandler()
{
    for (auto &item : fd_map)
    {
        close(item.first);
        close(item.second);
    }
}

void ServerHandler::SetTime(int timer_fd)
{
    struct itimerspec timer;
    memset(&timer, 0, sizeof(timer));
    timer.it_value.tv_sec = 1;
    timerfd_settime(timer_fd, 0, &timer, NULL);
}

int ServerHandler::HandleConnect(int client_socket)
{
    int timer_fd;
    timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
    assert(timer_fd != -1);

    SetTime(timer_fd);

    {
        std::lock_guard<std::mutex> guard(m);
        fd_map[timer_fd] = client_socket;
    }

    printf("hc connection num: %d\n", ++cnt_connet);
    printf("client_socket: %d timer_fd: %d\n", client_socket, timer_fd);

    return timer_fd;
}

int ServerHandler::HandleTime(int timer_fd)
{
    assert(fd_map.count(timer_fd));
    char send_msg[6] = "ping";
    int expect_len = 4, recv_len = 0;
    int recv_cnt;
    int client_socket;
    const int BUF_SIZE = 1024;
    char buf[BUF_SIZE];
    
    {
        std::lock_guard<std::mutex> guard(m);
        client_socket = fd_map[timer_fd];
    }

    uint64_t exp;
    ssize_t size = read(timer_fd, &exp, sizeof(uint64_t));
    assert(size == sizeof(uint64_t));

    write(client_socket, send_msg, strlen(send_msg));

    printf("%d\n", ++cnt_time);
    if (cnt_connet > 1 && cnt_time % cnt_connet == 0)
    {
        printf("connection num: %d\n", cnt_connet);
    }
    printf("send msg: %s\n", send_msg);

    int cnt = 0;
    while (recv_len < expect_len)
    {
        recv_cnt = read(client_socket, &buf[recv_len], BUF_SIZE);
        recv_len += recv_cnt;
        assert(recv_len < 10);
        if (cnt++ > 10 || recv_cnt == -1)
        {
            printf("client %d closed\n", client_socket);
            close(client_socket);
            close(timer_fd);
            fd_map.erase(timer_fd);
            return -1;
        }
    }
    buf[recv_len] = '\0';
    printf("recv msg: %s\n", buf);

    SetTime(timer_fd);

    return 1;
}