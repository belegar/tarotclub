#ifndef THREADQUEUE_H
#define THREADQUEUE_H

#include <iostream>
#include <thread>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <chrono>

template<typename Data>
class ConcurrentQueue
{

public:
    void Push(Data const& data)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mQueue.push(data);
        mMutex.unlock();
        mCondVar.notify_one();
    }

    bool Empty() const
    {
        std::lock_guard<std::mutex> lock(mMutex);
        return mQueue.empty();
    }

    bool TryPop(Data& popped_value)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        if(mQueue.empty())
        {
            return false;
        }

        popped_value = mQueue.front();
        mQueue.pop();
        return true;
    }

    void WaitAndPop(Data& popped_value)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        while(mQueue.empty())
        {
            mCondVar.wait(lock);
        }

        popped_value = mQueue.front();
        mQueue.pop();
    }

private:
    std::queue<Data> mQueue;
    mutable std::mutex mMutex;
    std::condition_variable mCondVar;

};


#endif // THREADQUEUE_H
