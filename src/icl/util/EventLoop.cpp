#include "EventLoop.h"
#include "IEventLoop.h"

static time_t Next(time_t current, std::uint32_t period)
{
    return ((current / period) * period) + period;
}

/*****************************************************************************/
EventLoop::EventLoop()
    : mStopRequested(false)
{

}
/*****************************************************************************/
EventLoop::~EventLoop()
{
    Stop();
}
/*****************************************************************************/
void EventLoop::Stop()
{
    mQueue.Push(EvStop);
    mThread.join();
}
/*****************************************************************************/
void EventLoop::AddTimer(uint32_t period, CallBack callBack)
{
    Timer tm;
    time_t rawtime;

    time(&rawtime);

    tm.period = period;
    tm.callBack = callBack;
    tm.next = Next(rawtime, period);

    mTimers.push_back(tm);
}
/*****************************************************************************/
void EventLoop::Register(Event event, IEventLoop::CallBack callBack)
{
    (void) event;
    (void) callBack;
}
/*****************************************************************************/
void EventLoop::SendEvent(Event event)
{
    mQueue.Push(event);
}
/*****************************************************************************/
void EventLoop::Run()
{
    mThread = std::thread(EventLoop::EntryPoint, this);

    // Main loop, can be called in main() thread
    while(!mStopRequested)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1U));
        mQueue.Push(EvTimer);
    }
}
/*****************************************************************************/
void EventLoop::EntryPoint(void *pthis)
{
    EventLoop *pt = static_cast<EventLoop *>(pthis);
    pt->Loop();
}
/*****************************************************************************/
void EventLoop::UpdateTimers()
{
    time_t rawtime;

    time(&rawtime);

    for (auto &t : mTimers)
    {
        if (rawtime >= t.next)
        {
            t.next = Next(rawtime, t.period);
            t.callBack(EvTimer);
        }
    }
}
/*****************************************************************************/
void EventLoop::Loop()
{
    while(!mStopRequested)
    {
        std::uint32_t ev;
        mQueue.WaitAndPop(ev);

        if (ev == EvTimer)
        {
            UpdateTimers();
        }
        else if (ev == EvStop)
        {
            mStopRequested = true;
        }
        else if (ev >= EvUser)
        {
            //ev.callBack(ev.type);
        }
        else
        {
            // Nothing, forget this event
        }
    }
}
