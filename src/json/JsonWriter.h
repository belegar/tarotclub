/*=============================================================================
 * TarotClub - JsonWriter.h
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

#ifndef JSON_WRITER_H
#define JSON_WRITER_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include "JsonValue.h"

// Forward declarations to resolve inter-dependency between Array and Object
class JsonArray;
class JsonObject;

/*****************************************************************************/
class JsonArray : public IJsonNode
{
public:
    JsonArray(std::uint32_t level);
    virtual ~JsonArray();

    std::string ToString();

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

    std::string ToString();
    void CreateValuePair(const std::string &name, const JsonValue &value);
    JsonArray *CreateArrayPair(const std::string &name);
    JsonObject *CreateObjectPair(const std::string &name);

private:
    std::vector<std::pair<std::string, IJsonNode *> > mObject;
    std::uint32_t mLevel;
};
/*****************************************************************************/
class JsonWriter : public JsonObject
{
public:
    JsonWriter();
    ~JsonWriter();

    bool SaveToFile(const std::string &fileName);
};


#endif // JSON_WRITER_H

//=============================================================================
// End of file JsonWriter.h
//=============================================================================
