/*=============================================================================
 * TarotClub - ThreadQueue.h
 *=============================================================================
 * Generic, blocking queue of data, to be used within a thread
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#ifndef THREADQUEUE_H
#define THREADQUEUE_H

#include <iostream>
#include <thread>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <chrono>


template<typename Data>
class ThreadQueue
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

//=============================================================================
// End of file ThreadQueue.h
//=============================================================================
