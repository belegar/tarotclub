/*=============================================================================
 * TarotClub - JsonReader.h
 *=============================================================================
 * Wrapper class to read JSON values from a file
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

#ifndef JSON_READER_H
#define JSON_READER_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "JsonValue.h"

/*****************************************************************************/
class JsonReader : public JsonObject
{

public:
    enum ParseStatus
    {
        JSON_PARSE_OK,
        JSON_PARSE_BAD_NUMBER,
        JSON_PARSE_BAD_STRING,
        JSON_PARSE_BAD_IDENTIFIER,
        JSON_PARSE_STACK_OVERFLOW,
        JSON_PARSE_STACK_UNDERFLOW,
        JSON_PARSE_MISMATCH_BRACKET,
        JSON_PARSE_UNEXPECTED_CHARACTER,
        JSON_PARSE_UNQUOTED_KEY,
        JSON_PARSE_BREAKING_BAD
    };

    enum JsonTag
    {
        JSON_TAG_NUMBER = 0,
        JSON_TAG_STRING,
        JSON_TAG_BOOL,
        JSON_TAG_ARRAY,
        JSON_TAG_OBJECT,
        JSON_TAG_NULL = 0xF
    };

    // ctors / dtor
    JsonReader();
    ~JsonReader();

    // Helpers
    bool Open(const std::string &fileName);
    void Close();

    // Getters
    bool GetValue(const std::string &obj, const std::string &key, std::int32_t &value);
    bool GetValue(const std::string &obj, const std::string &key, std::string &value);
    bool GetValue(const std::string &obj, const std::string &key, bool &value);

private:
    bool mValid;

    JsonValue GetJsonValue(const std::string &obj, const std::string &key, JsonValue::ValueType type);

    JsonReader::ParseStatus Parse(char *s, char **endptr);

    double StringToDouble(char *s, char **endptr);


    /*****************************************************************************/
    inline bool IsDelim(char c)
    {
        return isspace(c) || c == ',' || c == ':' || c == ']' || c == '}' || c == '\0';
    }
    /*****************************************************************************/
    static inline int CharToInt(char c)
    {
        if (c >= 'a')
        {
            return c - 'a' + 10;
        }
        if (c >= 'A')
        {
            return c - 'A' + 10;
        }
        return c - '0';
    }
};

#endif // JSON_READER_H

//=============================================================================
// End of file JsonReader.h
//=============================================================================
