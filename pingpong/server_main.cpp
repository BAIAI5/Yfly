#include "pingpong_server.h"

#include <thread>

int main()
{
    Server server(9190);
    std::thread pingpong(&Server::run, &server);
    server.Accept();
    pingpong.join();
    return 0;
}