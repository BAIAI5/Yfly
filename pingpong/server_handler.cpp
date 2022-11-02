#include "server_handler.h"
class ServerHandler;

#include <sys/socket.h>
#include <sys/timerfd.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

//static int cnt_connet;
static int cnt_time;

ServerHandler::~ServerHandler()
{
    for (auto &item : time_client_map)
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

    {
        std::lock_guard<std::mutex> guard(m);
        time_client_map.insert({timer_fd, client_socket});
        client_time_map.insert({client_socket, timer_fd});
    }

    // time_client_map.insert({timer_fd, client_socket});
    // client_time_map.insert({client_socket, timer_fd});

    SetTime(timer_fd);

    return timer_fd;
}

int ServerHandler::HandleRead(int client_socket)
{
    const int BUF_SIZE = 10;
    char buf[BUF_SIZE];
    int expect_len = 4, recv_len = 0;
    int recv_cnt;
    int cnt = 0;

    while (recv_len < expect_len)
    {
        recv_cnt = read(client_socket, &buf[recv_len], BUF_SIZE);
        recv_len += recv_cnt;
        assert(recv_len < 10);
        if (cnt++ > 10 || recv_cnt == -1)
        {
            printf("client %d closed cnt: %d recv_cnt: %d\n", client_socket, cnt, recv_cnt);
            return -1;
        }
    }
    buf[recv_len] = '\0';

    struct timespec ts;
    if (client_socket == cnt_time)
    {
        printf("recv msg: %s\n", buf);
        clock_gettime(CLOCK_MONOTONIC, &ts);
        printf("client_socket: %d pingpong end: %ld\n\n", client_socket, ts.tv_sec);
    }

    int timer_fd;
    {
        std::lock_guard<std::mutex> guard(m);
        assert(client_time_map.count(client_socket));
        timer_fd = client_time_map[client_socket];
    }
    SetTime(timer_fd);

    return 1;
}

int ServerHandler::HandleTime(int timer_fd)
{
    char send_msg[6] = "ping";
    int client_socket;
    
    {
        std::lock_guard<std::mutex> guard(m);
        assert(time_client_map.count(timer_fd));
        client_socket = time_client_map[timer_fd];
    }
    // assert(time_client_map.count(timer_fd));
    // client_socket = time_client_map[timer_fd];

    uint64_t exp;
    ssize_t size = read(timer_fd, &exp, sizeof(uint64_t));
    assert(size == sizeof(uint64_t));

    if (cnt_time == 0) cnt_time = client_socket;

    struct timespec ts;
    if (client_socket == cnt_time)
    {
        clock_gettime(CLOCK_MONOTONIC, &ts);
        printf("client_socket: %d pingpong start: %ld\n", client_socket, ts.tv_sec);
    }

    write(client_socket, send_msg, strlen(send_msg));

    if (client_socket == cnt_time)
    	printf("send msg: %s\n", send_msg);

    return 1;
}

FileDescriptorType ServerHandler::FdType(int fd)
{
    if (time_client_map.count(fd) != 0) return TIMER_FD;
    if (client_time_map.count(fd) != 0) return CLIENT_SOCKET;
    return ERROR_FD;
}