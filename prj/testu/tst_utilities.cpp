#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include "Observer.h"
#include <cstdint>

static std::uint32_t gCounter = 0U;
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
    void DettachAndNotify();

private:
    Subj mySubject;
    Obs obsTable[cNbObservers];

};

Utilities::Utilities()
{
}


void Utilities::AttachAndNotify()
{
    std::uint32_t i;

    gCounter = 0U;

    // Add all observers
    for (i = 0; i < cNbObservers; i++)
    {
        mySubject.Attach(obsTable[i]);
    }

    // notify all
    mySubject.Notify(42U);

    // Test the good reception of the information
    for (i = 0; i < cNbObservers; i++)
    {
        QCOMPARE(obsTable[i].GetValue(), 42U);
    }

    QCOMPARE(gCounter, cNbObservers);
}

void Utilities::DettachAndNotify()
{
    std::uint32_t i;
    gCounter = 0U;

    // Remove some observers
    for (i = 0; i < cNbObserversRemoved; i++)
    {
        mySubject.Dettach(obsTable[i]);
    }

    // notify all
    mySubject.Notify(23U);

    // Last obervers have the new value
    for (i = cNbObserversRemoved; i < cNbObservers; i++)
    {
        QCOMPARE(obsTable[i].GetValue(), 23U);
    }

    // Number of notifiers must be correct
    QCOMPARE(gCounter, cNbObservers - cNbObserversRemoved);

    // First observers have kept the old value
    for (i = 0; i < cNbObserversRemoved; i++)
    {
        QCOMPARE(obsTable[i].GetValue(), 42U);
    }
}


QTEST_MAIN(Utilities)

#include "tst_utilities.moc"
