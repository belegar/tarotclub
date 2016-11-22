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
#include <map>
#include <cstdint>

// Forward declarations to resolve inter-dependency between Array and Object
class JsonArray;
class JsonObject;
class JsonValue;

/*****************************************************************************/
class JsonObject
{
public:
    JsonObject() {}
    JsonObject(const JsonObject &obj);

    std::string ToString(std::uint32_t level) const;
    bool HasValue(const std::string &key);
    JsonValue GetValue(const std::string &key) const;
    void Clear();
    void AddValue(const std::string &name, const JsonValue &value);
    bool ReplaceValue(const std::string &keyPath, const JsonValue &value);
    std::uint32_t GetSize() { return mObject.size(); }

    JsonObject &operator = (JsonObject const &rhs);

private:
    std::map<std::string, JsonValue> mObject;
};

/*****************************************************************************/
class JsonArray
{
public:
    std::string ToString(std::uint32_t level) const;
    void Clear();
    // JsonArray
    JsonValue GetEntry(std::uint32_t index);
    std::uint32_t Size() { return mArray.size(); }
    void AddValue(const JsonValue &value);
    bool ReplaceValue(const std::string &keyPath, const JsonValue &value);

    typedef std::vector<JsonValue>::iterator Iterator;
    Iterator Begin() { return mArray.begin(); }
    Iterator End() { return mArray.end(); }

private:
    std::vector<JsonValue> mArray;
};
/*****************************************************************************/
class JsonValue
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

    // From Value class
    JsonValue(std::int32_t value);
    JsonValue(std::uint32_t value);
    JsonValue(std::uint16_t value);
    JsonValue(std::uint8_t value);
    JsonValue(double value);
    JsonValue(const char *value);
    JsonValue(const std::string &value);
    JsonValue(bool value);
    JsonValue(const JsonValue &value);
    JsonValue(); // default constructor creates an invalid value!
    JsonValue(const JsonObject &obj);
    JsonValue(const JsonArray &array);

    // Implemented virtual methods from IJsonNode
    Tag GetTag() const
    {
        return mTag;
    }

    std::string ToString(std::uint32_t level) const;
    void Clear();

    JsonValue &operator = (JsonValue const &rhs);

    bool IsValid() const      { return mTag != INVALID; }
    bool IsArray() const      { return mTag == ARRAY; }
    bool IsObject() const     { return mTag == OBJECT; }
    bool IsNull() const       { return mTag == NULL_VAL; }
    bool IsString() const     { return mTag == STRING; }
    bool IsInteger() const    { return mTag == INTEGER; }
    bool IsBoolean() const    { return mTag == BOOLEAN; }
    bool IsDouble() const     { return mTag == DOUBLE; }

    JsonObject &GetObject() { return mObject; }
    JsonArray &GetArray() { return mArray; }

    std::int32_t    GetInteger() const   { return mIntegerValue; }
    double          GetDouble() const    { return mDoubleValue; }
    bool            GetBool() const      { return mBoolValue; }
    std::string     GetString() const    { return mStringValue; }

    bool GetValue(const std::string &nodePath, std::string &value) const;
    bool GetValue(const std::string &nodePath, std::uint32_t &value) const;
    bool GetValue(const std::string &nodePath, std::uint16_t &value) const;
    bool GetValue(const std::string &nodePath, std::int32_t &value) const;
    bool GetValue(const std::string &nodePath, bool &value) const;
    bool GetValue(const std::string &nodePath, double &value) const;

    void SetNull()
    {
        mTag = NULL_VAL;
    }

    /**
     * @brief Recursively try to find a value using the key path separated by ':' character
     * @param keyPath
     * @return
     */
    JsonValue FindValue(const std::string &keyPath) const;
    bool ReplaceValue(const std::string &keyPath, const JsonValue &value);

private:
    Tag mTag;
    JsonObject mObject;
    JsonArray mArray;
    std::int32_t mIntegerValue;
    double mDoubleValue;
    std::string mStringValue;
    bool mBoolValue;
};

#endif // JSONVALUE_H

//=============================================================================
// End of file JsonValue.h
//=============================================================================
