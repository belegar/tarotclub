#ifndef SEMAPHORE_H
#define SEMAPHORE_H


#include <mutex>
#include <condition_variable>
#include <chrono>

class Semaphore
{
public:
    Semaphore (int count = 0)
        : mCount(count)
    {

    }

    inline void Notify()
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mCount++;
        mCondVar.notify_one();
    }

    inline bool Wait(int ms = 0)
    {
        bool normal = true;
        std::unique_lock<std::mutex> lock(mMutex);

        while(mCount == 0)
        {
            if (ms > 0)
            {
                normal = (mCondVar.wait_for(lock, std::chrono::milliseconds(ms)) == std::cv_status::no_timeout);
            }
            else
            {
                mCondVar.wait(lock);
            }
        }
        mCount--;
        return normal;
    }

private:
    std::mutex mMutex;
    std::condition_variable mCondVar;
    int mCount;
};

#endif // SEMAPHORE_H
