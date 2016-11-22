#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>

#include "tst_json.h"
#include "Util.h"
#include "JsonReader.h"
#include "JsonWriter.h"

JsonTest::JsonTest()
{

}


void JsonTest::ParseFile()
{
    JsonValue json;

    std::string path = Util::ExecutablePath() + "/../../tests/input/test.json";

    if (!JsonReader::ParseFile(json, path))
    {
        std::stringstream ss;
        ss << "Cannot open file " << path;
        QFAIL(ss.str().c_str());
    }

    std::string value;
    // key is at the root of the document
    if (!json.GetValue("version", value))
    {
        QFAIL("Get value error");
    }

    std::cout << "version = " << value << std::endl;
    QCOMPARE(value, std::string("1.5"));

    value.clear();
    if (!json.GetValue("identity:name", value))
    {
        QFAIL("Get value error");
    }

    std::cout << "identity.name = " << value << std::endl;
    QCOMPARE(value, std::string("Moi"));

    std::int32_t intvalue = 0;
    if (!json.GetValue("delay_before_cleaning", intvalue))
    {
        QFAIL("Get value error");
    }
    std::cout << "delay_before_cleaning = " << intvalue << std::endl;
    QCOMPARE(intvalue, 1500);

    double doublevalue = 0;
    if (!json.GetValue("double_value", doublevalue))
    {
        QFAIL("Get double value error");
    }
    std::cout << "double_value = " << doublevalue << std::endl;
    QCOMPARE(doublevalue, 42.7809);

    // Testing Array of strings
    JsonValue array = json.FindValue("identity:array_of_strings");
    QCOMPARE(array.IsArray(), true);
    QCOMPARE(array.GetArray().Size(), 4U);

    for (std::uint32_t i = 0U; i < array.GetArray().Size(); i++)
    {
        JsonValue entry = array.GetArray().GetEntry(i);
        if (entry.IsValid())
        {
            if (entry.IsString())
            {
                std::cout << "Value " << i << ": " << entry.GetString() << std::endl;
            }
            else
            {
                QFAIL("Json value must be a string");
            }
        }
        else
        {
            QFAIL("Not a valid Json value!");
        }
    }

    // Testing array of objects
    QCOMPARE(json.FindValue("array_of_objects").GetArray().Size(), 2U);
}

void JsonTest::ModifyValue()
{
    JsonValue json;

    std::string path = Util::ExecutablePath() + "/../../tests/input/test.json";

    if (!JsonReader::ParseFile(json, path))
    {
        std::stringstream ss;
        ss << "Cannot open file " << path;
        QFAIL(ss.str().c_str());
    }

    JsonValue newDouble(807.24);
    QCOMPARE(json.ReplaceValue("double_value", newDouble), true);

    // Replace value with a path
    JsonValue newString("Juju");
    QCOMPARE(json.ReplaceValue("identity:name", newString), true);

    // Replace value within an array
    JsonValue newAddress("www.whatever.com");
    QCOMPARE(json.ReplaceValue("array_of_objects:1:address", newAddress), true);
    std::cout << json.ToString(0U) << std::endl;
}

void JsonTest::GenerateFile()
{
    JsonObject root;

    root.AddValue("toto", "titi");
    root.AddValue("tutu", "tata");

    JsonObject first;
    first.AddValue("prout", 42);
    root.AddValue("first", first);

    JsonObject second;
    second.AddValue("prat", false);
    second.AddValue("prut", -3400);
    second.AddValue("prot", 807.53);
    second.AddValue("escaped", R"(hey\Ho!)");
    root.AddValue("second", second);

    JsonArray array;
    array.AddValue("my");
    array.AddValue("name");
    array.AddValue("is");
    array.AddValue("john");
    array.AddValue("doe");
    root.AddValue("MyArray", array);

    std::cout << root.ToString(0U) << std::endl;

    QCOMPARE(JsonWriter::SaveToFile(root, "essai.json"), true);
}

