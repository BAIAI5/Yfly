#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 1024

void error_handling(char * buf);

int main(int argc, char *argv[])
{
    int sock;
    char message[BUF_SIZE];
    int str_len, recv_len, recv_cnt;

    struct sockaddr_in serv_adr;

    if (argc != 3)
    {
        printf("usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error");
    else
        puts("Connected......");
    
    while (1)
    {
        str_len = 0;

        int expect_len = 4;

        recv_len = 0;
        while (recv_len < expect_len)
        {
            recv_cnt = read(sock, &message[recv_len], BUF_SIZE);
            if (recv_cnt == -1)
                error_handling("read() error");
            recv_len += recv_cnt;
        }
        message[recv_len] = '\0';
        printf("client recv msg: %s\n", message);

        char *send_msg = "pong\n";
        write(sock, send_msg, strlen(send_msg));
        printf("client send msg: %s\n", send_msg);
    }
    close(sock);
    return 0;
}

void error_handling(char * buf)
{
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}