#ifndef OBSERVER_H
#define OBSERVER_H

#include <list>
#include <algorithm>

template <class T>
class Observer
{
public:
    virtual void Update(const T &info) = 0;
};

template <class T>
class Subject
{

public:
    void Attach(Observer<T> &observer)
    {
        mList.push_back(&observer);
    }

    void Dettach(Observer<T> &observer)
    {
        mList.remove(&observer);
    }

    void Notify(const T &info)
    {
        for (typename std::list< Observer<T> *>::iterator it = mList.begin(); it != mList.end(); ++it)
        {
            (*it)->Update(info);
        }
    }

private:
    std::list< Observer<T> *> mList;
};


#endif // OBSERVER_H
