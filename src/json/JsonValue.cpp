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
JsonArray::JsonArray(std::uint32_t level)
    : mLevel(level)
{

}
/*****************************************************************************/
JsonArray::~JsonArray()
{
    for (std::uint32_t i = 0U; i < mArray.size(); i++)
    {
        IJsonNode *node = mArray[i];
        delete node;
    }
    mArray.clear();
}
/*****************************************************************************/
std::string JsonArray::ToString()
{
    std::string text = "[\n";
    std::string indent = CreateIndent(mLevel + 1U);

    std::uint32_t index = 0U;
    for (std::vector<IJsonNode *>::iterator iter = mArray.begin(); iter != mArray.end(); ++iter)
    {
        text += indent + (*iter)->ToString();
        index++;
        if (index < mArray.size())
        {
            text += ",\n";
        }
    }

    indent = CreateIndent(mLevel);
    text += "\n" + indent + "]";
    return text;
}
/*****************************************************************************/
void JsonArray::CreateValue(const JsonValue &value)
{
    JsonValue *val = new JsonValue(value);
    mArray.push_back(val);
}
/*****************************************************************************/
JsonObject *JsonArray::CreateObject()
{
    JsonObject *obj = new JsonObject(1U);
    mArray.push_back(obj);
    return obj;
}
/*****************************************************************************/
JsonArray *JsonArray::CreateArray()
{
    JsonArray *array = new JsonArray(1U);
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

    for (std::uint32_t i = 0U; i < mObject.size(); i++)
    {
        std::pair<std::string, IJsonNode *> node = mObject[i];
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
    std::string text = "{\n";
    std::string indent = CreateIndent(mLevel + 1U);

    std::uint32_t index = 0U;
    for (std::uint32_t i = 0U; i < mObject.size(); i++)
    {
        std::pair<std::string, IJsonNode *> node = mObject[i];

        text += indent + "\"" + node.first + "\": " + node.second->ToString();
        index++;
        if (index < mObject.size())
        {
            text += ",\n";
        }
    }
    indent = CreateIndent(mLevel);
    text += "\n" + indent + "}";
    return text;
}
/*****************************************************************************/
void JsonObject::CreateValuePair(const std::string &name, const JsonValue &value)
{
    JsonValue *val = new JsonValue(value);
    std::pair<std::string, IJsonNode *> node(name, val);
    mObject.push_back(node);
}
/*****************************************************************************/
JsonArray *JsonObject::CreateArrayPair(const std::string &name)
{
    JsonArray *array = new JsonArray(mLevel + 1U);
    std::pair<std::string, IJsonNode *> node(name, array);
    mObject.push_back(node);
    return array;
}
/*****************************************************************************/
JsonObject *JsonObject::CreateObjectPair(const std::string &name)
{
    JsonObject *obj = new JsonObject(mLevel + 1U);
    std::pair<std::string, IJsonNode *> node(name, obj);
    mObject.push_back(node);
    return obj;
}
/*****************************************************************************/
bool JsonObject::HasNode(const std::string &key)
{
    bool ret = false;
    std::vector<std::pair<std::string, IJsonNode *> >::const_iterator iter;
    for (iter = mObject.begin(); iter != mObject.end(); ++iter)
    {
        if (iter->first == key)
        {
            ret = true;
        }
    }
    return ret;
}
/*****************************************************************************/
IJsonNode *JsonObject::GetNode(const std::string &key)
{
    IJsonNode *retval = NULL;
    std::vector<std::pair<std::string, IJsonNode *> >::const_iterator iter;

    for (iter = mObject.begin(); iter != mObject.end(); ++iter)
    {
        if (iter->first == key)
        {
            retval = iter->second;
        }
    }

    return retval;
}
/*****************************************************************************/
std::string JsonValue::ToString()
{
    std::string text;
    std::stringstream ss;

    if (GetType() == STRING)
    {
        text = "\"" + GetString() + "\"";
    }
    else if (GetType() == INTEGER)
    {
        ss << GetInteger();
        text = ss.str();
    }
    else if (GetType() == BOOLEAN)
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
    else if (GetType() == DOUBLE)
    {
        ss << GetDouble();
        text = ss.str();
    }
    else if (GetType() == NULL_VAL)
    {
        text = "null";
    }
    else
    {
        // INVALID!
    }

    return text;
}

//=============================================================================
// End of file JsonValue.cpp
//=============================================================================
