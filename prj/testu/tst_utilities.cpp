#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>

#include "Observer.h"
#include "ByteArray.h"
#include "ByteStreamReader.h"
#include "ByteStreamWriter.h"
#include "tst_utilities.h"
#include "Util.h"

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
    ByteStreamReader stream_rd(block);
    ByteStreamWriter stream_wr(block);

    // Step 1 ---------------------------------------
    // Basic test: serialize a byte and deserialize it
    val8_in = 0x42U;
    stream_wr << val8_in;
    std::uint32_t size = 1U;

    QCOMPARE(block.Size(), size);
    stream_rd >> val8_out;

    QCOMPARE(val8_in, val8_out);


    // Step 2 ---------------------------------------
    stream_rd.Seek(0U);
    stream_wr.Seek(0U);

    val32_in = 0xAABBCCDD;
    stream_wr << val32_in;

    std::cout << stream_rd.ToString();

    val8_in = 0x23;
    stream_wr.Seek(1U);
    stream_wr << val8_in;

    std::cout << stream_rd.ToString();

    stream_rd >> val32_out;
    val32_in = 0xAABB23DD;
    QCOMPARE(val32_in, val32_out);


    // Step 3 ---------------------------------------
    stream_rd.Seek(0U);
    stream_wr.Seek(0U);

    std::string hello = "Hello, world!";
    stream_wr << hello;
    std::string reply;
    stream_rd >> reply;

    if (hello != reply)
    {
        std::cout << "Stream output: " << reply << std::endl;
        QFAIL("Strings are not the same!");
    }

    ByteArray sub = block.SubArray(7, 6);

    std::string substr = sub.ToSring();
    std::cout << "Subarray output: " << substr << std::endl;
}

void Utilities::TestUtilFunctions()
{
    bool ret;
    std::string rootPath;

#ifdef USE_WINDOWS_OS
    rootPath = "c:/";
#else
    rootPath = "/tmp/";
#endif

    std::string path1 = rootPath + "tetstor";
    std::string path2 = rootPath + "test_mkdir/sub/directory/";

    std::cout << Util::ExecutablePath() << std::endl;
    std::cout << Util::HomePath() << std::endl;

    ret = Util::Mkdir(path1);
    QCOMPARE(ret, true);
    ret = Util::FolderExists(path1);
    QCOMPARE(ret, true);

    ret = Util::Mkdir(path2);
    QCOMPARE(ret, true);
    ret = Util::FolderExists(path2);
    QCOMPARE(ret, true);

    std::cout << "Current date-time: " << Util::CurrentDateTime("%Y-%m-%d.%X")  << std::endl;
}

