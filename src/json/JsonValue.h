/*=============================================================================
 * TarotClub - JsonValue.h
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

#ifndef JSONVALUE_H
#define JSONVALUE_H

#include "JSValue.h"
#include <vector>


// Forward declarations to resolve inter-dependency between Array and Object
class JsonArray;
class JsonObject;
class JsonValue;

/*****************************************************************************/
class IJsonNode
{
public:
    enum Tag
    {
        JSON_VALUE,
        JSON_ARRAY,
        JSON_OBJECT
    };

    virtual ~IJsonNode() {}

    virtual Tag GetTag() = 0;
    virtual std::string ToString() = 0;
};
/*****************************************************************************/
class JsonArray : public IJsonNode
{
public:
    JsonArray(std::uint32_t level);
    virtual ~JsonArray();

    // Implemented virtual methods
    IJsonNode::Tag GetTag() { return IJsonNode::JSON_ARRAY; }
    std::string ToString();

    // JsonArray
    void CreateValue(const JsonValue &value);
    JsonArray *CreateArray(); // no name in an array inside an array
    JsonObject *CreateObject(); // no name in an object inside an object

private:
    std::vector<IJsonNode *> mArray;
    std::uint32_t mLevel;
};
/*****************************************************************************/
class JsonObject : public IJsonNode
{
public:
    JsonObject(std::uint32_t level);
    virtual ~JsonObject();

    // Implemented virtual methods
    IJsonNode::Tag GetTag() { return IJsonNode::JSON_OBJECT; }
    std::string ToString();

    // JsonObject
    void CreateValuePair(const std::string &name, const JsonValue &value);
    JsonArray *CreateArrayPair(const std::string &name);
    JsonObject *CreateObjectPair(const std::string &name);

private:
    std::vector<std::pair<std::string, IJsonNode *> > mObject;
    std::uint32_t mLevel;
};
/*****************************************************************************/
class JsonValue : public JSValue, public IJsonNode
{

public:

    // From JSValue
    JsonValue(std::int32_t value) : JSValue(value) {}
    JsonValue(double value) : JSValue(value) {}
    JsonValue(const char *value) : JSValue(value) {}
    JsonValue(const std::string &value) : JSValue(value) {}
    JsonValue(bool value) : JSValue(value) {}
    JsonValue(const JSValue &value) : JSValue(value) {}
    JsonValue() : JSValue() {}

    // Implemented virtual methods from IJsonNode
    IJsonNode::Tag GetTag() { return IJsonNode::JSON_VALUE; }
    std::string ToString();
};

#endif // JSONVALUE_H

//=============================================================================
// End of file JsonValue.h
//=============================================================================
