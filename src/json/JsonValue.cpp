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
    Clear();
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
bool JsonArray::HasNode(const std::string &key)
{
    bool ret = false;
    std::vector<IJsonNode *>::const_iterator iter;
    for (iter = mArray.begin(); iter != mArray.end(); ++iter)
    {
        // Broadcast the request to each underneath node
        if((*iter)->HasNode(key))
        {
            ret = true;
        }
    }
    return ret;
}
/*****************************************************************************/
IJsonNode *JsonArray::GetNode(const std::string &key)
{
    (void) key;
    return nullptr;
}
/*****************************************************************************/
void JsonArray::Clear()
{
    for (std::uint32_t i = 0U; i < mArray.size(); i++)
    {
        IJsonNode *node = mArray[i];
        delete node;
    }
    mArray.clear();
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
    JsonObject *obj = new JsonObject(mLevel + 1U);
    mArray.push_back(obj);
    return obj;
}
/*****************************************************************************/
IJsonNode *JsonArray::GetEntry(std::uint32_t index)
{
    IJsonNode *node = nullptr;

    if (index < mArray.size())
    {
        node = mArray[index];
    }
    return node;
}
/*****************************************************************************/
JsonArray *JsonArray::CreateArray()
{
    JsonArray *array = new JsonArray(mLevel + 1U);
    mArray.push_back(array);
    return array;
}
/*****************************************************************************/

//          *                          *                                  *

/*****************************************************************************/
JsonObject::JsonObject(std::uint32_t level)
    : mLevel(level)
{

}
/*****************************************************************************/
JsonObject::~JsonObject()
{
    Clear();
}
/*****************************************************************************/
void JsonObject::Clear()
{

#ifdef JSON_DEBUG
    std::uint32_t counter = 0U;
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

//          *                          *                                  *

/*****************************************************************************/
std::string JsonValue::ToString()
{
    std::string text;
    std::stringstream ss;

    if (GetTag() == STRING)
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
    else
    {
        // INVALID!
    }

    return text;
}
/*****************************************************************************/
JsonValue::JsonValue(std::int32_t value)
    : mTag(INTEGER)
    , mObject(nullptr)
    , mArray(nullptr)
    , mIntegerValue(value)
    , mDoubleValue(0.0F)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(double value)
    : mTag(DOUBLE)
    , mObject(nullptr)
    , mArray(nullptr)
    , mIntegerValue(0)
    , mDoubleValue(value)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(const char *value)
    : mTag(STRING)
    , mObject(nullptr)
    , mArray(nullptr)
    , mIntegerValue(0)
    , mDoubleValue(0.0F)
    , mStringValue(value)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(const std::string &value)
    : mTag(STRING)
    , mObject(nullptr)
    , mArray(nullptr)
    , mIntegerValue(0)
    , mDoubleValue(0.0F)
    , mStringValue(value)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(bool value)
    : mTag(BOOLEAN)
    , mObject(nullptr)
    , mArray(nullptr)
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
JsonValue::JsonValue(JsonObject *obj)
    : mTag(OBJECT)
    , mObject(obj)
    , mArray(nullptr)
    , mIntegerValue(0)
    , mDoubleValue(0.0F)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue(JsonArray *array)
    : mTag(ARRAY)
    , mObject(nullptr)
    , mArray(array)
    , mIntegerValue(0)
    , mDoubleValue(0.0F)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::JsonValue()
    : mTag(INVALID)
    , mObject(nullptr)
    , mArray(nullptr)
    , mIntegerValue(0)
    , mDoubleValue(0.0F)
    , mBoolValue(false)
{

}
/*****************************************************************************/
JsonValue::~JsonValue()
{

}
/*****************************************************************************/
bool JsonValue::FromNode(IJsonNode *node, std::int32_t &value)
{
    bool ret = false;
    if (node != NULL)
    {
        if ((node->GetTag() != IJsonNode::OBJECT) &&
            (node->GetTag() != IJsonNode::ARRAY))
        {
            // we are arrived to the value, get it
            JsonValue *jsonValue = dynamic_cast<JsonValue *>(node);
            if (jsonValue !=  nullptr)
            {
                if (jsonValue->IsValid() && (jsonValue->GetTag() == IJsonNode::INTEGER))
                {
                    value = jsonValue->GetInteger();
                    ret = true;
                }
            }
        }
    }
    return ret;
}
/*****************************************************************************/
bool JsonValue::FromNode(IJsonNode *node, std::uint32_t &value)
{
    bool ret = false;
    if (node != NULL)
    {
        if ((node->GetTag() != IJsonNode::OBJECT) &&
            (node->GetTag() != IJsonNode::ARRAY))
        {
            // we are arrived to the value, get it
            JsonValue *jsonValue = dynamic_cast<JsonValue *>(node);
            if (jsonValue !=  nullptr)
            {
                if (jsonValue->IsValid() && (jsonValue->GetTag() == IJsonNode::INTEGER))
                {
                    value = static_cast<std::uint32_t>(jsonValue->GetInteger());
                    ret = true;
                }
            }
        }
    }
    return ret;
}
/*****************************************************************************/
bool JsonValue::FromNode(IJsonNode *node, std::uint16_t &value)
{
    bool ret = false;
    if (node != NULL)
    {
        if ((node->GetTag() != IJsonNode::OBJECT) &&
            (node->GetTag() != IJsonNode::ARRAY))
        {
            // we are arrived to the value, get it
            JsonValue *jsonValue = dynamic_cast<JsonValue *>(node);
            if (jsonValue !=  nullptr)
            {
                if (jsonValue->IsValid() && (jsonValue->GetTag() == IJsonNode::INTEGER))
                {
                    value = static_cast<std::uint16_t>(jsonValue->GetInteger());
                    ret = true;
                }
            }
        }
    }
    return ret;
}
/*****************************************************************************/
bool JsonValue::FromNode(IJsonNode *node, std::string &value)
{
    bool ret = false;
    if (node != NULL)
    {
        if ((node->GetTag() != IJsonNode::OBJECT) &&
            (node->GetTag() != IJsonNode::ARRAY))
        {
            // we are arrived to the value, get it
            JsonValue *jsonValue = dynamic_cast<JsonValue *>(node);
            if (jsonValue !=  nullptr)
            {
                if (jsonValue->IsValid() && (jsonValue->GetTag() == IJsonNode::STRING))
                {
                    value = jsonValue->GetString();
                    ret = true;
                }
            }
        }
    }
    return ret;
}
/*****************************************************************************/
bool JsonValue::FromNode(IJsonNode *node, bool &value)
{
    bool ret = false;
    if (node != NULL)
    {
        if ((node->GetTag() != IJsonNode::OBJECT) &&
            (node->GetTag() != IJsonNode::ARRAY))
        {
            // we are arrived to the value, get it
            JsonValue *jsonValue = dynamic_cast<JsonValue *>(node);
            if (jsonValue !=  nullptr)
            {
                if (jsonValue->IsValid() && (jsonValue->GetTag() == IJsonNode::BOOLEAN))
                {
                    value = jsonValue->GetBool();
                    ret = true;
                }
            }
        }
    }
    return ret;
}
/*****************************************************************************/
bool JsonValue::FromNode(IJsonNode *node, double &value)
{
    bool ret = false;
    if (node != NULL)
    {
        if ((node->GetTag() != IJsonNode::OBJECT) &&
            (node->GetTag() != IJsonNode::ARRAY))
        {
            // we are arrived to the value, get it
            JsonValue *jsonValue = dynamic_cast<JsonValue *>(node);
            if (jsonValue !=  nullptr)
            {
                if (jsonValue->IsValid() && (jsonValue->GetTag() == IJsonNode::DOUBLE))
                {
                    value = jsonValue->GetDouble();
                    ret = true;
                }
            }
        }
    }
    return ret;
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
bool JsonValue::HasNode(const std::string &key)
{
    // A value does not have any key
    (void) key;
    return false;
}
/*****************************************************************************/
IJsonNode *JsonValue::GetNode(const std::string &key)
{
    // A value does not have any sub-node
    (void) key;
    return nullptr;
}
/*****************************************************************************/
void JsonValue::Clear()
{
    mTag = INVALID;

    if (IsObject())
    {
        mObject->Clear();
    }
    if (IsArray())
    {
        mArray->Clear();
    }
}

//=============================================================================
// End of file JsonValue.cpp
//=============================================================================
