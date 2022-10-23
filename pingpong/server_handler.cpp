#include "server_handler.h"

#include <sys/socket.h>
#include <sys/timerfd.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

void ServerHandler::SetTime(int timer_fd)
{
    struct itimerspec timer;
    memset(&timer, 0, sizeof(timer));
    timer.it_value.tv_sec = 1;
    timerfd_settime(timer_fd, 0, &timer, NULL);
}

int ServerHandler::HandleConnect(int server_fd)
{
    int client_socket = accept(server_fd, NULL, NULL);
    assert(client_socket != -1);

    int timer_fd;
    timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
    assert(timer_fd != -1);

    SetTime(timer_fd);

    fd_map[timer_fd] = client_socket;

    return timer_fd;
}

void ServerHandler::HandleTime(int timer_fd)
{
    assert(fd_map.count(timer_fd));
    char *send_msg = "ping";
    int expect_len = 4, recv_len = 0;
    int recv_cnt;
    int client_socket = fd_map[timer_fd];
    const int BUF_SIZE = 1024;
    char buf[BUF_SIZE];

    uint64_t exp;
    ssize_t size = read(timer_fd, &exp, sizeof(uint64_t));
    assert(size != sizeof(uint64_t));

    write(client_socket, send_msg, strlen(send_msg));

    while (recv_len < expect_len)
    {
        recv_cnt = read(client_socket, &buf[recv_len], BUF_SIZE);
        assert(recv_cnt != -1);
        recv_len += recv_cnt;
        assert(recv_len < 10);
        buf[recv_len] = '\0';
    }

    SetTime(timer_fd);
}