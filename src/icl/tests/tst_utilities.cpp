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
#include "Zip.h"

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


void Utilities::TestByteArray()
{
    // Test 1: test constructors
    char initArray[] = { 42, 28, (char)255, 0, (char)203, 107, 4, (char)189, 95 };
    std::string initString = "abcdef";

    std::uint32_t expected_size = sizeof(initArray);
    ByteArray array(initArray, expected_size);

    // Check if the ByteArray and init arrays contents are equals
    QCOMPARE(array.Size(), expected_size);
    for (std::uint32_t i = 0U; i < expected_size; i++)
    {
        QCOMPARE(static_cast<std::uint8_t>(initArray[i]), array.Get(i));
    }

    expected_size = initString.size();
    ByteArray array2(initString);

    QCOMPARE(array2.Size(), expected_size);
    for (std::uint32_t i = 0U; i < expected_size; i++)
    {
        QCOMPARE(static_cast<std::uint8_t>(initString.at(i)), array2.Get(i));
    }

    // Test 2: try to access data out of the array boundaries: default behavior is to return zero
    std::uint8_t value = array.Get(807);
    std::uint8_t expected_value = 0U;
    QCOMPARE(expected_value, value);

    // Write out of bounds (not failing but in a dummy area)
    value = 7U;
    expected_value = 0U;
    array.Put(25U, value);
    QCOMPARE(static_cast<std::uint32_t>(expected_value), static_cast<std::uint32_t>(array.Get(25U)));

    array.Clear();
    expected_value = 0U;
    value = array.Get(4);
    QCOMPARE(expected_value, value);

    // Test 3: clear and erase some data
    expected_size = 0U;
    QCOMPARE(array.Size(), expected_size);

    // Should not work
    expected_size = array2.Size();
    array2.Erase(3, 7);
    QCOMPARE(array2.Size(), expected_size);

    // Should work
    expected_size = 3U;
    array2.Erase(3, 3);
    QCOMPARE(array2.Size(), expected_size);

    // Test 4: append operator
    // Before appending, the array size is 3 bytes length
    std::string coucou = "coucou";
    ByteArray array3(coucou);
    array2 += array3;
    expected_size = 3 + coucou.size();
    QCOMPARE(array2.Size(), expected_size);

    // We verify that the resulted string is correct
    QCOMPARE(coucou, array2.SubArray(3, coucou.size()).ToSring());
    std::string expected_string = "abc";
    QCOMPARE(expected_string, array2.SubArray(0, expected_string.size()).ToSring());

    // Test 4: word and dword random access
    ByteArray packet;
    packet.PushBack(0x21);
    packet.PushBack(0xB2);
    std::uint16_t expected_word = 45601U;
    QCOMPARE(expected_word, packet.GetUint16(0U));

    packet.PushBack(0xD8);
    packet.PushBack(0x60);
    packet.PushBack(0xF2);
    packet.PushBack(0x86);
    std::uint32_t expected_dword = 2264031448UL;
    QCOMPARE(expected_dword, packet.GetUint32(2U));

}

void Utilities::TestByteStream()
{
    std::uint8_t val8_in, val8_out;
    std::uint32_t val32_in, val32_out;
    std::uint16_t val16_in, val16_out;

    ByteArray block;
    ByteStreamReader stream_rd(block);
    ByteStreamWriter stream_wr(block);

    // ------------------------------------------------------------------------
    // Step 1: Basic test, serialize a byte and deserialize it
    val8_in = 0x42U;
    val8_out = 0U;
    stream_wr << val8_in;
    std::uint32_t size = 1U;

    QCOMPARE(block.Size(), size);
    stream_rd >> val8_out;

    QCOMPARE(val8_in, val8_out);

    block.Clear();
    stream_rd.Seek(0U);
    stream_wr.Seek(0U);

    val16_in = 0xABCD;
    val16_out = 0U;
    stream_wr << val16_in;
    size = 2U;
    QCOMPARE(block.Size(), size);
    stream_rd >> val16_out;

    QCOMPARE(val16_in, val16_out);

    // ------------------------------------------------------------------------
    // Step 2: Serialize some integers
    stream_rd.Seek(0U);
    stream_wr.Seek(0U);

    val32_in = 0xAABBCCDD;
    stream_wr << val32_in;

    std::string expected_string = "[dd, cc, bb, aa]";
    std::string actual_string = stream_rd.ToString();
    std::cout << actual_string << std::endl;
    QCOMPARE(expected_string, actual_string);

    val8_in = 0x23;
    stream_wr.Seek(1U);
    stream_wr << val8_in;

    expected_string = "[dd, 23, bb, aa]";
    actual_string = stream_rd.ToString();
    std::cout << actual_string << std::endl;
    QCOMPARE(expected_string, actual_string);

    stream_rd >> val32_out;
    val32_in = 0xAABB23DD;
    QCOMPARE(val32_in, val32_out);


    // ------------------------------------------------------------------------
    // Step 3 string serialization
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

    expected_string = "lo, wo";
    actual_string = sub.ToSring();
    std::cout << "Subarray output: " << actual_string << std::endl;
    QCOMPARE(expected_string, actual_string);

    // ------------------------------------------------------------------------
    // Step 4 boolean serialization
    stream_rd.Seek(0U);
    stream_wr.Seek(0U);
    bool expected_bool = true;
    stream_wr << expected_bool;
    bool actual_bool;
    stream_rd >> actual_bool;
    QCOMPARE(expected_bool, actual_bool);

    block.Clear();
    stream_rd.Seek(0U);
    stream_wr.Seek(0U);
    expected_bool = false;
    stream_wr << expected_bool;
    stream_rd >> actual_bool;
    QCOMPARE(expected_bool, actual_bool);

    // ------------------------------------------------------------------------
    // Step 5: try seeking out of block size

    block.Clear();
    stream_wr.Seek(28U);
    expected_string = "notmodified";
    actual_string = "tatuto";
    stream_wr << actual_string;
    // Here, the stream reader is not reset: the string is not touched
    actual_string = expected_string; // both are equal
    stream_rd >> actual_string;
    QCOMPARE(expected_string, actual_string);

    // Seek to valid offset
    expected_string = "tatuto";
    actual_string.clear();
    stream_rd.Seek(0U);
    stream_rd >> actual_string;
    QCOMPARE(expected_string, actual_string);

    // Read again with an offset out of bounds
    stream_rd.Seek(37U);
    expected_string = "notmodified";
    actual_string = expected_string;
    stream_rd >> actual_string;
    QCOMPARE(expected_string, actual_string);
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


void Utilities::TestZip()
{
    Zip zip;

    QCOMPARE(zip.Open(Util::ExecutablePath() + "/../../bin/aicontest/ai.zip", true), true);

}
