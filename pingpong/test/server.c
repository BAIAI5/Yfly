#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>

#define BUF_SIZE 100
#define EPOLL_SIZE 50
#define PINGPONG_SIZE 50

typedef struct Time
{
    int clnt_sock, timerfd;
}pingpong_event;

void error_handling(char * buf);

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t adr_sz;
    int str_len, i;
    char buf[BUF_SIZE];

    struct epoll_event *ep_events;
    struct epoll_event event;
    int epfd, event_cnt;

    pingpong_event pingpong;

    int timerfd;

    if (argc != 2)
    {
        printf("usage: %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");
    
    epfd = epoll_create(EPOLL_SIZE);
    ep_events = malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

    event.events = EPOLLIN;
    event.data.fd = serv_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    puts("before while");
    int pingpong_cnt = 0;
    while (1) {
        int cnt = 0;
        puts("before wait");
        event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if (event_cnt == -1)
        {
            puts("epoll_wait() error");
            break;
        }
        puts("after wait");

        for (i = 0; i < event_cnt; ++i)
        {
            if (ep_events[i].data.fd == serv_sock)
            {
                puts("1");
                adr_sz = sizeof(clnt_adr);
                clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_adr, &adr_sz);

                
                struct itimerspec timer;
                timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
                memset(&timer, 0, sizeof(timer));
                timer.it_value.tv_sec = 1;
                timer.it_interval.tv_sec = 1;
                timerfd_settime(timerfd, 0, &timer, NULL);

                pingpong.clnt_sock = clnt_sock;
                pingpong.timerfd = timerfd;

                event.events = EPOLLIN;
                event.data.ptr = (void *)&pingpong;
                epoll_ctl(epfd, EPOLL_CTL_ADD, pingpong.timerfd, &event);
                printf("connected client: %d\ntfd: %d\n", clnt_sock, timerfd);
            }
            else
            {
                puts("2");
                str_len = 0;
                char *send_msg = "ping";
                int expect_len = 4, recv_len = 0;
                int recv_cnt;

                pingpong_event *p_sock = (pingpong_event *)ep_events[i].data.ptr;

                uint64_t exp;
                ssize_t size = read(p_sock->timerfd, &exp, sizeof(uint64_t));
                if(size != sizeof(uint64_t)) {
                    error_handling("read timerfd error!");
                }

                write(p_sock->clnt_sock, send_msg, strlen(send_msg));
                printf("pingpong cnt: %d\n", ++pingpong_cnt);
                printf("server send msg: %s\n", send_msg);

                cnt = 0;
                while (recv_len < expect_len)
                {
                    if (cnt > 10) break;
                    recv_cnt = read(p_sock->clnt_sock, &buf[recv_len], BUF_SIZE);
                    if (recv_cnt == -1)
                        error_handling("read() error");
                    recv_len += recv_cnt;
                    cnt ++;
                }

                if (cnt > 10) break;

                buf[recv_len] = '\0';
                printf("server recv msg: %s\n\n", buf);
            }
        }
        if (cnt > 10) break;
    }
    close(serv_sock);
    close(epfd);
    close(pingpong.clnt_sock);
    close(pingpong.timerfd);
    free(ep_events);
    printf("before return\n");
    return 0;
}

void error_handling(char * buf)
{
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}
