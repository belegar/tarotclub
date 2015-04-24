/*=============================================================================
 * TarotClub - JsonValue.cpp
 *=============================================================================
 * Modelization of a JSON generic value
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

#include "JsonValue.h"
#include <sstream>

// Helper function
static std::string CreateIndent(std::uint32_t level)
{
    std::string indent;

    for (std::uint32_t i = 0U; i < (4U * level); i++)
    {
        indent += " ";
    }
    return indent;
}
/*****************************************************************************/
std::string JsonArray::ToString(std::uint32_t level)
{
    std::string text = "[\n";
    std::string indent = CreateIndent(level + 1U);

    std::uint32_t index = 0U;
    for (std::vector<JsonValue>::iterator iter = mArray.begin(); iter != mArray.end(); ++iter)
    {
        text += indent + iter->ToString(level + 1U);
        index++;
        if (index < mArray.size())
        {
            text += ",\n";
        }
    }

    indent = CreateIndent(level);
    text += "\n" + indent + "]";
    return text;
}
/*****************************************************************************/
void JsonArray::Clear()
{
    mArray.clear();
}
/*****************************************************************************/
void JsonArray::AddValue(const JsonValue &value)
{
    mArray.push_back(value);
}
/*****************************************************************************/
JsonValue JsonArray::GetEntry(std::uint32_t index)
{
    JsonValue value;

    if (index < mArray.size())
    {
        value = mArray[index];
    }
    return value;
}
/*****************************************************************************/

//          *                          *                                  *

/*****************************************************************************/
void JsonObject::Clear()
{
    mObject.clear();
}
/*****************************************************************************/
std::string JsonObject::ToString(std::uint32_t level)
{
    std::string text = "{\n";
    std::string indent = CreateIndent(level + 1U);

    std::uint32_t index = 0U;

    for (std::map<std::string, JsonValue>::iterator it = mObject.begin(); it != mObject.end(); ++it)
    {
        text += indent + "\"" + it->first + "\": " + it->second.ToString(level + 1U);
        index++;
        if (index < mObject.size())
        {
            text += ",\n";
        }
    }
    indent = CreateIndent(level);
    text += "\n" + indent + "}";
    return text;
}
/*****************************************************************************/
void JsonObject::AddValue(const std::string &name, const JsonValue &value)
{
    mObject[name] = value;
}
/*****************************************************************************/
bool JsonObject::HasValue(const std::string &key)
{
    bool ret = false;
    for (std::map<std::string, JsonValue>::iterator it = mObject.begin(); it != mObject.end(); ++it)
    {
        if (it->first == key)
        {
            ret = true;
        }
    }
    return ret;
}
/*****************************************************************************/
JsonValue JsonObject::GetValue(const std::string &key)
{
    JsonValue value;
    for (std::map<std::string, JsonValue>::iterator it = mObject.begin(); it != mObject.end(); ++it)
    {
        if (it->first == key)
        {
            value = it->second;
        }
    }

    return value;
}
/*****************************************************************************/

//          *                          *                                  *

/*****************************************************************************/
std::string JsonValue::ToString(std::uint32_t level)
{
    std::string text;
    std::stringstream ss;

    if (IsString())
    {
        text = "\"" + GetString() + "\"";
    }
    else if (GetTag() == INTEGER)
    {
        ss << GetInteger();
        text = ss.str();
    }
    else if (GetTag() == BOOLEAN)
    {
        if (GetBool())
        {
            text = "true";
        }
        else
        {
            text = "false";
        }
    }
    else if (GetTag() == DOUBLE)
    {
        ss << GetDouble();
        text = ss.str();
    }
    else if (GetTag() == NULL_VAL)
    {
        text = "null";
    }
    else if (IsObject())
    {
        text = mObject.ToString(level);
    }
    else
    {
        text = mArray.ToString(level);
    }

    return text;
}
/*****************************************************************************/
JsonValue::JsonValue(std::int32_t value)
    : mTag(INTEGER)
    , mIntegerValue(value)
    , mDoubleValue(0.0F)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(double value)
    : mTag(DOUBLE)
    , mIntegerValue(0)
    , mDoubleValue(value)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(const char *value)
    : mTag(STRING)
    , mIntegerValue(0)
    , mDoubleValue(0.0F)
    , mStringValue(value)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(const std::string &value)
    : mTag(STRING)
    , mIntegerValue(0)
    , mDoubleValue(0.0F)
    , mStringValue(value)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(bool value)
    : mTag(BOOLEAN)
    , mIntegerValue(0)
    , mDoubleValue(0.0F)
    , mBoolValue(value)
{

}
/*****************************************************************************/
JsonValue::JsonValue(const JsonValue &value)
{
    *this = value;
}
/*****************************************************************************/
JsonValue::JsonValue(const JsonObject &obj)
    : mTag(OBJECT)
    , mIntegerValue(0)
    , mDoubleValue(0.0F)
    , mBoolValue(false)
{
    mObject = obj;
}
/*****************************************************************************/
JsonValue::JsonValue(const JsonArray &array)
    : mTag(ARRAY)
    , mIntegerValue(0)
    , mDoubleValue(0.0F)
    , mBoolValue(false)
{
    mArray = array;
}
/*****************************************************************************/
JsonValue::JsonValue()
    : mTag(INVALID)
    , mIntegerValue(0)
    , mDoubleValue(0.0F)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue &JsonValue::operator =(const JsonValue &rhs)
{
    mTag = rhs.mTag;
    mObject = rhs.mObject;
    mArray = rhs.mArray;
    mIntegerValue = rhs.mIntegerValue;
    mDoubleValue = rhs.mDoubleValue;
    mStringValue = rhs.mStringValue;
    mBoolValue = rhs.mBoolValue;
    return *this;
}
/*****************************************************************************/
void JsonValue::Clear()
{
    mTag = INVALID;
    mObject.Clear();
    mArray.Clear();
}
/*****************************************************************************/
bool JsonValue::GetValue(const std::string &nodePath, std::string &value)
{
    bool ret = false;

    JsonValue json = FindValue(nodePath);
    if (json.IsString())
    {
        value = json.GetString();
        ret = true;
    }

    return ret;
}
/*****************************************************************************/
bool JsonValue::GetValue(const std::string &nodePath, std::uint32_t &value)
{
    bool ret = false;

    JsonValue json = FindValue(nodePath);
    if (json.IsInteger())
    {
        value = static_cast<std::uint32_t>(json.GetInteger());
        ret = true;
    }

    return ret;
}
/*****************************************************************************/
bool JsonValue::GetValue(const std::string &nodePath, std::uint16_t &value)
{
    bool ret = false;

    JsonValue json = FindValue(nodePath);
    if (json.IsInteger())
    {
        value = static_cast<std::uint16_t>(json.GetInteger());
        ret = true;
    }

    return ret;
}
/*****************************************************************************/
bool JsonValue::GetValue(const std::string &nodePath, std::int32_t &value)
{
    bool ret = false;

    JsonValue json = FindValue(nodePath);
    if (json.IsInteger())
    {
        value = json.GetInteger();
        ret = true;
    }

    return ret;
}
/*****************************************************************************/
bool JsonValue::GetValue(const std::string &nodePath, bool &value)
{
    bool ret = false;

    JsonValue json = FindValue(nodePath);
    if (json.IsBoolean())
    {
        value = json.GetBool();
        ret = true;
    }

    return ret;
}
/*****************************************************************************/
bool JsonValue::GetValue(const std::string &nodePath, double &value)
{
    bool ret = false;

    JsonValue json = FindValue(nodePath);
    if (json.IsDouble())
    {
        value = json.GetDouble();
        ret = true;
    }

    return ret;
}
/*****************************************************************************/
JsonValue JsonValue::FindValue(const std::string &keyPath)
{
    std::vector<std::string> keys = Split(keyPath);

    JsonValue temp = mObject;
    for (std::uint32_t i = 0U; i < keys.size(); i++)
    {
        if (temp.IsObject())
        {
            if (temp.GetObject().HasValue(keys[i]))
            {
                temp = temp.GetObject().GetValue(keys[i]);
            }
        }
        else
        {
            break;
        }
    }
    return temp;
}
/*****************************************************************************/
std::vector<std::string> JsonValue::Split(const std::string &obj)
{
    std::vector<std::string> path;
    std::size_t found = std::string::npos;
    int pos = 0;

    do
    {
        int size;
        found = obj.find(':', pos);
        if (found != std::string::npos)
        {
            // calculate size of the string between the delimiters
            size = found - pos;
        }
        else
        {
            // last: get remaining characters
            size = obj.size() - pos;
        }

        std::string key = obj.substr(pos, size);
        pos = found + 1;
        path.push_back(key);
    }
    while (found != std::string::npos);
    return path;
}

//=============================================================================
// End of file JsonValue.cpp
//=============================================================================
