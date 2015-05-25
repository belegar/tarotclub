/*=============================================================================
 * TarotClub - Observer.h
 *=============================================================================
 * Implementation of the observer design pattern, templated version
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

#ifndef OBSERVER_H
#define OBSERVER_H

#include <list>
#include <algorithm>

template <class T>
class Observer
{
public:
    Observer(std::uint8_t mask = 0xFFU)
        : mMask(mask)
    {

    }
    void SetMask(std::uint8_t mask) { mMask = mask; }
    std::uint8_t GetMask() { return mMask; }

    virtual void Update(const T &info) = 0;

private:
    std::uint8_t mMask;
};

template <class T>
class Subject
{

public:
    void Attach(Observer<T> &observer)
    {
        mList.push_back(&observer);
    }

    void Detach(Observer<T> &observer)
    {
        mList.remove(&observer);
    }

    void Notify(const T &info, std::uint8_t flags = 0xFFU)
    {
        for (typename std::list< Observer<T> *>::iterator it = mList.begin(); it != mList.end(); ++it)
        {
            if (((*it)->GetMask() & flags) != 0U)
            {
                (*it)->Update(info);
            }
        }
    }

private:
    std::list< Observer<T> *> mList;
};


#endif // OBSERVER_H

//=============================================================================
// End of file Observer.h
//=============================================================================
