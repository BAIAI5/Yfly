#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <unordered_map>
#include <mutex>

class ServerHandler
{
public:
    ServerHandler()
    {
        
    }
    ~ServerHandler();
    int HandleConnect(int server_fd);
    int HandleTime(int timer_fd);
private:
    void SetTime(int timer_fd);
    std::unordered_map<int, int> fd_map;
    std::mutex m;
};

#endif // EVENT_HANDLER_H