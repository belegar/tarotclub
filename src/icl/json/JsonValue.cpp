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
#include "Util.h"
#include <sstream>
#include <regex>

/*****************************************************************************/
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
std::string JsonArray::ToString(std::uint32_t level) const
{
    std::string text = "[\n";
    std::string indent = CreateIndent(level + 1U);

    std::uint32_t index = 0U;
    for (std::vector<JsonValue>::const_iterator iter = mArray.begin(); iter != mArray.end(); ++iter)
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
bool JsonArray::ReplaceValue(const std::string &keyPath, const JsonValue &value)
{
    bool ret = false;
    std::uint32_t index = 0U;

    std::vector<std::string> keys = Util::Split(keyPath, ":");
    std::istringstream(keys[0]) >> index;
    keys.erase(keys.begin());

    if (index < Size())
    {
        if (keys.size() == 0)
        {
            mArray[index] = value;
            ret = true;
        }
        else
        {
            // Forward deeper
            ret = mArray[index].ReplaceValue(Util::Join(keys, ":"), value);
        }
    }

    return ret;
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
JsonObject::JsonObject(const JsonObject &obj)
{
    *this = obj;
}
/*****************************************************************************/
JsonObject &JsonObject::operator = (JsonObject const &rhs)
{
    mObject = rhs.mObject;
    return *this;
}
/*****************************************************************************/
std::string JsonObject::ToString(std::uint32_t level) const
{
    std::string text = "{\n";
    std::string indent = CreateIndent(level + 1U);

    std::uint32_t index = 0U;

    for (std::map<std::string, JsonValue>::const_iterator it = mObject.begin(); it != mObject.end(); ++it)
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
bool JsonObject::ReplaceValue(const std::string &keyPath, const JsonValue &value)
{
    bool ret = false;
    std::vector<std::string> keys = Util::Split(keyPath, ":");
    std::string name = keys[0];
    keys.erase(keys.begin());

    for (std::map<std::string, JsonValue>::iterator it = mObject.begin(); it != mObject.end(); ++it)
    {
        if (it->first == name)
        {
            if (keys.size() == 0)
            {
                // The key is in this object, replace it with the new value
                it->second = value;
                ret = true;
            }
            else
            {
                ret = it->second.ReplaceValue(Util::Join(keys, ":"), value); // go deeper
            }
        }
    }

    return ret;
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
JsonValue JsonObject::GetValue(const std::string &key) const
{
    JsonValue value;
    for (std::map<std::string, JsonValue>::const_iterator it = mObject.begin(); it != mObject.end(); ++it)
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
std::string JsonValue::ToString(std::uint32_t level) const
{
    std::string text;
    std::stringstream ss;

    if (IsString())
    {
        std::regex reg("\\\\");
        std::string escapedString = std::regex_replace(GetString(), reg, "\\\\");
        text = "\"" + escapedString + "\"";
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
JsonValue::JsonValue(std::uint32_t value)
    : mTag(INTEGER)
    , mIntegerValue(value)
    , mDoubleValue(0.0F)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(std::uint16_t value)
    : mTag(INTEGER)
    , mIntegerValue(value)
    , mDoubleValue(0.0F)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(std::uint8_t value)
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
bool JsonValue::GetValue(const std::string &nodePath, std::string &value) const
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
bool JsonValue::GetValue(const std::string &nodePath, std::uint32_t &value) const
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
bool JsonValue::GetValue(const std::string &nodePath, std::uint16_t &value) const
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
bool JsonValue::GetValue(const std::string &nodePath, std::int32_t &value) const
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
bool JsonValue::GetValue(const std::string &nodePath, bool &value) const
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
bool JsonValue::GetValue(const std::string &nodePath, double &value) const
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
JsonValue JsonValue::FindValue(const std::string &keyPath) const
{
    std::vector<std::string> keys = Util::Split(keyPath, ":");

    JsonValue temp = *this;
    for (std::uint32_t i = 0U; i < keys.size(); i++)
    {
        if (temp.IsObject())
        {
            if (temp.GetObj().HasValue(keys[i]))
            {
                temp = temp.GetObj().GetValue(keys[i]);
            }
        }
        else if (temp.IsArray())
        {
            for (JsonArray::Iterator iter = temp.GetArray().Begin(); iter != temp.GetArray().End(); ++iter)
            {
                if (iter->IsObject())
                {
                    if (iter->GetObj().HasValue(keys[i]))
                    {
                        temp = iter->GetObj().GetValue(keys[i]);
                        break;
                    }
                }
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
bool JsonValue::ReplaceValue(const std::string &keyPath, const JsonValue &value)
{
    bool ret = false;

    if (IsObject())
    {
        ret = GetObj().ReplaceValue(keyPath, value);
    }
    else if (IsArray())
    {
        ret = GetArray().ReplaceValue(keyPath, value);
    }
    else
    {
        // Nothing
    }

    return ret;
}

//=============================================================================
// End of file JsonValue.cpp
//=============================================================================
