#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <unordered_map>
#include <mutex>

enum FileDescriptorType
{
    TIMER_FD = 0,
    CLIENT_SOCKET,
    ERROR_FD = -1,
};

class ServerHandler
{
public:
    ServerHandler()
    {
        
    }
    ~ServerHandler();
    int HandleConnect(int client_socket);
    int HandleRead(int client_socket);
    int HandleTime(int timer_fd);
    FileDescriptorType FdType(int fd);
private:
    void SetTime(int timer_fd);
    std::unordered_map<int, int> time_client_map, client_time_map;
    std::mutex m;
};

#endif // EVENT_HANDLER_H