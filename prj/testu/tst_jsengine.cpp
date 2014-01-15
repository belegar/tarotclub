#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>

#include "defines.h"
#include "tst_jsengine.h"
#include "Util.h"
#include "JsonReader.h"

extern "C" void my_panic_handler(int code, const char *msg)
{
    /* Your panic handling.  Must not return. */
    volatile int i = 0;
    i++;
    while(1);
}

JsonTest::JsonTest()
{

}


void JsonTest::ParseFile()
{
    JsonReader json;

    std::string path = Util::ExecutablePath() + "/tarotclub.json";

    json.Open(path);
    std::string value;
    json.GetValue("version", value); // key is at the root of the document

    std::cout << "version = " << value << std::endl;
    value.clear();

    json.GetValue("identity", "name", value);

    std::cout << "identity.version = " << value << std::endl;
}

