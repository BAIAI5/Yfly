#ifndef PINGPONG_CLIENT_H
#define PINGPONG_CLIENT_H

#include <vector>
#include <string>

class Client
{
public:
    Client(std::vector<std::string> & ip);
    ~Client();
    void Connect(const char * server_ip, int server_port);
    void Respond();
private:
    void EventRegister(int event_fd);
    int PingPong(int sock);
    std::vector<int> sockets;
    std::vector<std::string> client_ip;
    int epoll_fd;
    std::vector<struct epoll_event> events;
    static const int max_connection;
};

#endif // PINGPONG_CLIENT_H