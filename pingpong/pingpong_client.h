#ifndef PINGPONG_CLIENT_H
#define PINGPONG_CLIENT_H

#include <sys/epoll.h>
#include <vector>
#include <string>

class Client
{
public:
    Client(std::vector<std::string> & ip, int num, int connect_num);
    ~Client();
    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete; 
    void Connect(const char * server_ip, int server_port);
    void Respond(int index);
private:
    void EventRegister(int epoll_fd, int event_fd);
    int PingPong(int sock);
    std::vector<std::vector<int>> sockets;
    std::vector<std::string> client_ip;
    std::vector<int> epoll_fds;
    std::vector<std::vector<struct epoll_event>> events;
    int thread_num;
    int max_connection;
    int connection_num;
};

#endif // PINGPONG_CLIENT_H