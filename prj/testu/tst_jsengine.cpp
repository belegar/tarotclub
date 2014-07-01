#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>

#include "tst_jsengine.h"
#include "Util.h"
#include "JsonReader.h"
#include "JsonWriter.h"

JsonTest::JsonTest()
{

}


void JsonTest::ParseFile()
{
    JsonReader json;

    std::string path = Util::ExecutablePath() + "/../prj/testu/test.json";

    if (!json.Open(path))
    {
        std::stringstream ss;
        ss << "Cannot open file " << path;
        QFAIL(ss.str().c_str());
    }

    std::string value;
    // key is at the root of the document
    if (!json.GetValue("", "version", value))
    {
        QFAIL("Get value error");
    }

    std::cout << "version = " << value << std::endl;
    QCOMPARE(value, std::string("1.5"));

    value.clear();
    if (!json.GetValue("identity", "name", value))
    {
        QFAIL("Get value error");
    }

    std::cout << "identity.name = " << value << std::endl;
    QCOMPARE(value, std::string("Moi"));

    std::int32_t intvalue = 0;
    if (!json.GetValue("", "delay_before_cleaning", intvalue))
    {
        QFAIL("Get value error");
    }
    std::cout << "delay_before_cleaning = " << intvalue << std::endl;
    QCOMPARE(intvalue, 1500);
}

void JsonTest::GenerateFile()
{
    JsonWriter json;

    json.CreateValuePair("toto", "titi");
    json.CreateValuePair("tutu", "tata");
    JsonObject *obj = json.CreateObjectPair("first");
    obj->CreateValuePair("prout", 42);

    JsonObject *obj2 = obj->CreateObjectPair("second");
    obj2->CreateValuePair("prat", false);
    obj2->CreateValuePair("prut", -3400);
    obj2->CreateValuePair("prot", 807.53);

    JsonArray *obj3 = json.CreateArrayPair("MyArray");
    obj3->CreateValue("my");
    obj3->CreateValue("name");
    obj3->CreateValue("is");
    obj3->CreateValue("john");
    obj3->CreateValue("doe");

    json.SaveToFile("essai.json");
}

