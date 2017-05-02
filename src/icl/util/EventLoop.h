#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <functional>
#include <vector>
#include "ThreadQueue.h"
#include "IEventLoop.h"

class EventLoop : public IEventLoop
{
public:
    EventLoop();
    ~EventLoop();

    void Run();
    void Stop();

    // From IEventLoop
    void AddTimer(std::uint32_t period, CallBack callBack);
    void Register(IEventLoop::Event event, CallBack callBack);
    void SendEvent(IEventLoop::Event event);

private:
    bool mStopRequested;
    std::thread mThread;
    ThreadQueue<std::uint32_t> mQueue;

    struct Timer
    {
        std::uint32_t period;
        CallBack callBack;
        time_t next;
    };

    std::vector<Timer> mTimers;
    //std::vector<

    void Loop();
    static void EntryPoint(void *pthis);
    void UpdateTimers();
};

#endif // EVENT_LOOP_H
