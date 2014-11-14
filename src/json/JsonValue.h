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

#include <string>
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
        INVALID,
        ARRAY,
        OBJECT,
        INTEGER,
        DOUBLE,
        BOOLEAN,
        STRING,
        NULL_VAL
    };

    virtual ~IJsonNode() {}

    virtual Tag GetTag() = 0;
    virtual bool HasNode(const std::string &key) = 0;
    virtual IJsonNode* GetNode(const std::string &key) = 0;
    virtual std::string ToString() = 0;
};
/*****************************************************************************/
class JsonArray : public IJsonNode
{
public:
    JsonArray(std::uint32_t level);
    virtual ~JsonArray();

    // Implemented virtual methods
    IJsonNode::Tag GetTag()
    {
        return IJsonNode::ARRAY;
    }
    std::string ToString();

    bool HasNode(const std::string &key);
    IJsonNode* GetNode(const std::string &key);

    // JsonArray
    IJsonNode *GetEntry(std::uint32_t index);
    std::uint32_t GetSize() { return mArray.size(); }
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
    IJsonNode::Tag GetTag()
    {
        return IJsonNode::OBJECT;
    }
    std::string ToString();
    bool HasNode(const std::string &key);
    IJsonNode *GetNode(const std::string &key);

    // JsonObject
    void CreateValuePair(const std::string &name, const JsonValue &value);
    JsonArray *CreateArrayPair(const std::string &name);
    JsonObject *CreateObjectPair(const std::string &name);

private:
    std::vector<std::pair<std::string, IJsonNode *> > mObject;
    std::uint32_t mLevel;
};
/*****************************************************************************/
class JsonValue : public IJsonNode
{
public:
    // From Value class
    JsonValue(std::int32_t value);
    JsonValue(double value);
    JsonValue(const char *value);
    JsonValue(const std::string &value);
    JsonValue(bool value);
    JsonValue(const JsonValue &value);
    JsonValue(); // default constructor creates an invalid value!
    JsonValue(JsonObject *obj);
    JsonValue(JsonArray *array);
    ~JsonValue();

    // Implemented virtual methods from IJsonNode
    IJsonNode::Tag GetTag()
    {
        return mTag;
    }

    std::string ToString();
    bool HasNode(const std::string &key);
    IJsonNode *GetNode(const std::string &key);

    JsonValue &operator = (JsonValue const &rhs);

    bool IsValid()
    {
        return mTag != INVALID;
    }

    bool IsArray() { return mArray != nullptr; }
    bool IsObject() { return mObject != nullptr; }
    bool IsNull() { return mTag == NULL_VAL; }

    JsonObject *GetObject() { return mObject; }
    JsonArray *GetArray() { return mArray; }

    std::int32_t    GetInteger()
    {
        return mIntegerValue;
    }
    double          GetDouble()
    {
        return mDoubleValue;
    }
    bool            GetBool()
    {
        return mBoolValue;
    }
    std::string     GetString()
    {
        return mStringValue;
    }

    void SetNull()
    {
        mTag = NULL_VAL;
    }

private:
    IJsonNode::Tag mTag;
    JsonObject *mObject;
    JsonArray *mArray;
    std::int32_t mIntegerValue;
    double mDoubleValue;
    std::string mStringValue;
    bool mBoolValue;
};

#endif // JSONVALUE_H

//=============================================================================
// End of file JsonValue.h
//=============================================================================
