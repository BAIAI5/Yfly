#include "pingpong_client.h"

#include <thread>

int main()
{
    std::vector<std::string> client_ip{"192.168.28.129"};
    Client client(client_ip);
    std::thread pingpong(&Client::Respond, &client);
    client.Connect("127.0.0.1", 9190);
    pingpong.join();
    return 0;
}