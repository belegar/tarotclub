#ifndef TST_UTILITIES_H
#define TST_UTILITIES_H

#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include "Observer.h"
#include "ByteStream.h"
#include <cstdint>

static const std::uint32_t cNbObservers = 10U;
static const std::uint32_t cNbObserversRemoved = 3U;

class Obs : public Observer<std::uint32_t>
{

public:
    Obs()
        : mValue(0U)
    {

    }

    virtual void Update(const std::uint32_t &info)
    {
        mValue = info;
        gCounter++;
    }

    std::uint32_t GetValue()
    {
        return mValue;
    }

    static std::uint32_t gCounter;

private:
    std::uint32_t mValue;
};

typedef Subject<std::uint32_t> Subj;

class Utilities : public QObject
{
    Q_OBJECT

public:
    Utilities();

private Q_SLOTS:
    void AttachAndNotify();
    void DetachAndNotify();
    void TestByteStream();

private:
    Subj mySubject;
    Obs obsTable[cNbObservers];

};

#endif // TST_UTILITIES_H
