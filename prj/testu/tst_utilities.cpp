#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include "Observer.h"
#include "ByteStream.h"
#include <cstdint>

#include "tst_utilities.h"

std::uint32_t Obs::gCounter = 0U;


Utilities::Utilities()
{
}


void Utilities::AttachAndNotify()
{
    std::uint32_t i;

    Obs::gCounter = 0U;

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

    QCOMPARE(Obs::gCounter, cNbObservers);
}

void Utilities::DetachAndNotify()
{
    std::uint32_t i;
    Obs::gCounter = 0U;

    // Remove some observers
    for (i = 0; i < cNbObserversRemoved; i++)
    {
        mySubject.Detach(obsTable[i]);
    }

    // notify all
    mySubject.Notify(23U);

    // Last obervers have the new value
    for (i = cNbObserversRemoved; i < cNbObservers; i++)
    {
        QCOMPARE(obsTable[i].GetValue(), 23U);
    }

    // Number of notifiers must be correct
    QCOMPARE(Obs::gCounter, cNbObservers - cNbObserversRemoved);

    // First observers have kept the old value
    for (i = 0; i < cNbObserversRemoved; i++)
    {
        QCOMPARE(obsTable[i].GetValue(), 42U);
    }
}


void Utilities::TestByteStream()
{
    std::uint8_t val8_in, val8_out;
    std::uint32_t val32_in, val32_out;

    ByteArray block;
    ByteStream stream(block);

    // Basic test: serialize a byte and deserialize it
    val8_in = 0x42U;
    stream << val8_in;
    QCOMPARE(block.size(), 1U);
    stream >> val8_out;
    QCOMPARE(block.size(), 0U);

    QCOMPARE(val8_in, val8_out);

    val32_in = 0xAABBCCDD;
    stream << val32_in;
    stream.Print();

    val8_in = 0x23;
    stream.Seek(1U);
    stream << val8_in;
    stream.Print();

    stream >> val32_out;

    val32_in = 0xAABB23DD;
    QCOMPARE(val32_in, val32_out);

    std::string hello = "Hello, world!";
    stream << hello;
    std::string reply;
    stream >> reply;

    if (hello != reply)
    {
        std::cout << "Stream output: " << reply << std::endl;
        QFAIL("Strings are not the same!");
    }
}

