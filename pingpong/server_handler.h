#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <unordered_map>

enum EventType
{
    kConnectEvent = 0,
    kTimeEvent,
};

class EventHandler
{
public:
    EventHandler();
    ~EventHandler();
    int HandleConnect(int server_fd);
    void HandleTime(int timer_fd);
private:
    void SetTime(int timer_fd);
    std::unordered_map<int, int> fd_map;
};

#endif // EVENT_HANDLER_H