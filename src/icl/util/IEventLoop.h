#ifndef I_EVENT_LOOP_H
#define I_EVENT_LOOP_H


#include <functional>
#include <cstdint>

class IEventLoop
{
public:
    enum Event {EvNothing, EvTimer, EvStop, EvUser = 100};

    typedef std::function<void (Event)> CallBack;

    ~IEventLoop() {}

    virtual void AddTimer(std::uint32_t period, CallBack callBack) = 0;
    virtual void Register(Event event, CallBack callBack) = 0;
    virtual void SendEvent(Event event) = 0;
};


#endif // I_EVENT_LOOP_H
