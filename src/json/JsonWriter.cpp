/*=============================================================================
 * TarotClub - JsonWriter.cpp
 *=============================================================================
 * Wrapper class to write JSON values to a file
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

#include <fstream>
#include <iostream>
#include <sstream>
#include "JsonWriter.h"

/*****************************************************************************/
JsonValue::JsonValue(std::int32_t value)
    : mType(INTEGER)
    , mIntegerValue(value)
{

}
/*****************************************************************************/
JsonValue::JsonValue(double value)
    : mType(DOUBLE)
    , mDoubleValue(value)
{

}
/*****************************************************************************/
JsonValue::JsonValue(const char * value)
    : mType(STRING)
    , mStringValue(value)
{

}
/*****************************************************************************/
JsonValue::JsonValue(const std::string &value)
    : mType(STRING)
    , mStringValue(value)
{

}
/*****************************************************************************/
JsonValue::JsonValue(bool value)
    : mType(BOOLEAN)
    , mBoolValue(value)
{

}
/*****************************************************************************/
JsonValue::JsonValue(const JsonValue &value)
{
    *this = value;
}
/*****************************************************************************/
JsonValue::JsonValue()
{
    mType = INVALID;
}
/*****************************************************************************/
JsonValue::~JsonValue()
{

}
/*****************************************************************************/
std::string JsonValue::ToString()
{
    std::string text;
    std::stringstream ss;

    if (mType == STRING)
    {
       text += "\"" + mStringValue + "\"";
    }
    else if (mType == INTEGER)
    {
        ss << mIntegerValue;
        text += ss.str();
    }
    else if (mType == BOOLEAN)
    {
        if (mBoolValue)
        {
            text = "true";
        }
        else
        {
            text = "false";
        }
    }
    else if (mType == DOUBLE)
    {
        ss << mDoubleValue;
        text += ss.str();
    }

    return text;
}
/*****************************************************************************/
JsonValue &JsonValue::operator =(const JsonValue &rhs)
{
    mType = rhs.mType;
    mIntegerValue = rhs.mIntegerValue;
    mDoubleValue = rhs.mDoubleValue;
    mStringValue = rhs.mStringValue;
    mBoolValue = rhs.mBoolValue;
    return *this;
}
/*****************************************************************************/
JsonArray::~JsonArray()
{
    for (std::uint32_t i = 0; i < mArray.size(); i++)
    {
        JsonNode *node = mArray[i];
        delete node;
    }
    mArray.clear();
}
/*****************************************************************************/
std::string JsonArray::ToString()
{
    std::string text;

    return text;
}
/*****************************************************************************/
JsonObject *JsonArray::CreateObject()
{
    JsonObject *obj = new JsonObject(1);
    mArray.push_back(obj);
    return obj;
}
/*****************************************************************************/
JsonArray *JsonArray::CreateArray()
{
    JsonArray *array = new JsonArray();
    mArray.push_back(array);
    return array;
}
/*****************************************************************************/
JsonObject::JsonObject(std::uint32_t level)
    : mLevel(level)

{

}
/*****************************************************************************/
JsonObject::~JsonObject()
{
#ifdef JSON_DEBUG
    static std::uint32_t counter = 0U;
#endif

    for (std::uint32_t i = 0; i < mObject.size(); i++)
    {
        std::pair<std::string, JsonNode *> node = mObject[i];
        delete node.second;
#ifdef JSON_DEBUG
        counter++;
#endif
    }
    mObject.clear();

#ifdef JSON_DEBUG
    std::cout << "Destroyed " << counter << " objects." << std::endl;
#endif
}
/*****************************************************************************/
std::string JsonObject::ToString()
{
    std::string text;
    std::string indent;

    for (std::uint32_t i = 0; i < (4*(mLevel+1)); i++)
    {
        indent += " ";
    }
    text += "{\n";

    std::uint32_t index = 0;
    for (std::uint32_t i = 0; i < mObject.size(); i++)
    {
        std::pair<std::string, JsonNode *> node = mObject[i];

        text += indent + "\"" + node.first + "\": " + node.second->ToString();
        index++;
        if (index < mObject.size())
        {
            text += ",\n";
        }
    }
    indent.clear();
    for (std::uint32_t i = 0; i < (4*mLevel); i++)
    {
        indent += " ";
    }
    text += "\n" + indent + "}";
    return text;
}
/*****************************************************************************/
void JsonObject::CreateValuePair(const std::string &name, const JsonValue &value)
{
    JsonValue *val = new JsonValue(value);
    std::pair<std::string, JsonNode *> node(name, val);
    mObject.push_back(node);
}
/*****************************************************************************/
JsonArray *JsonObject::CreateArrayPair(const std::string &name)
{
    JsonArray *array = new JsonArray();
    std::pair<std::string, JsonNode *> node(name, array);
    mObject.push_back(node);
    return array;
}
/*****************************************************************************/
JsonObject *JsonObject::CreateObjectPair(const std::string &name)
{
    JsonObject *obj = new JsonObject(mLevel+1);
     std::pair<std::string, JsonNode *> node(name, obj);
    mObject.push_back(node);
    return obj;
}
/*****************************************************************************/
JsonWriter::JsonWriter()
    : JsonObject(0U)
{

}
/*****************************************************************************/
JsonWriter::~JsonWriter()
{

}
/*****************************************************************************/
bool JsonWriter::SaveToFile(const std::string &fileName)
{
    std::ofstream f;
    f.open(fileName, std::ios_base::out | std::ios_base::binary);

    if (f.is_open())
    {
        f << ToString() << std::endl;
        f.close();
        return true;
    }
    return false;
}


//=============================================================================
// End of file JsonWriter.cpp
//=============================================================================
