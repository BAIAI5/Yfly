#include "pingpong_server.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>

int main(int argc, char *argv[])
{
    int thread_num = 16, port;
    char c;
    while ((c = getopt(argc,argv,"p:")) != -1)
    {
        switch(c)
        {
        case 'p':
            port = atoi(optarg);
            break;
        default:
            printf("usage: %s -p [port]\n", argv[0]);
            exit(1);
        }
    }

    if (port == 0)
    {
        printf("usage: %s -p [port]\n", argv[0]);
        exit(1);
    }

    Server server(port, thread_num);

    std::vector<std::thread> threads;
    for (int i = 0; i < thread_num; i++)
    {
        threads.push_back(std::thread(&Server::run, &server, i));
    }

    server.Accept();

    for (int i = 0; i < thread_num; i++)
    {
        threads[i].join();
    }
    return 0;
}
