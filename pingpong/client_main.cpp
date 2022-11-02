#include "pingpong_client.h"

#include <string.h>
#include <unistd.h>
#include <thread>

int main(int argc, char *argv[])
{
    char server_ip[20];
    int server_port, connect_num;
    char c;
    while ((c = getopt(argc,argv,"s:p:n:")) != -1)
    {
        switch(c)
        {
        case 's':
            strcpy(server_ip, optarg);
            break;
        case 'p':
            server_port = atoi(optarg);
            break;
        case 'n':
            connect_num = atoi(optarg);
            break;
        default:
            printf("usage: %s -p [port]\n", argv[0]);
            exit(1);
        }
    }

    if (server_port == 0 || connect_num == 0)
    {
        printf("usage: %s -s [IPADDR] -p [port] -n [NUM]\n", argv[0]);
        exit(1);
    }

    int thread_num = 16;
    std::vector<std::string> client_ip{"192.168.136.135", "192.168.136.136", "192.168.136.137", "192.168.136.138"};
    Client client(client_ip, thread_num, connect_num);

    std::vector<std::thread> threads;
    for (int i = 0; i < thread_num; i++)
    {
        threads.push_back(std::thread(&Client::Respond, &client, i));
    }

    client.Connect(server_ip, server_port);

    for (int i = 0; i < thread_num; i++)
    {
        threads[i].join();
    }

    return 0;
}
