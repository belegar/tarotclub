/*=============================================================================
 * TarotClub - JsonReader.cpp
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

#include <cctype>
#include <map>
#include <iostream>
#include <string.h>

#include "JsonReader.h"

// Uncomment this line to enable JSON parsing traces
//#define JSON_READER_D

/*****************************************************************************/
/**
 * @brief Open a JSON file for parsing
 *
 * After opening the JSON file, the context is ready for requests.
 *
 * @param fileName to open
 * @return true if the file has been parsed successfully
 */
bool JsonReader::ParseFile(JsonValue &json, const std::string &fileName)
{
    std::ifstream f;
    bool valid = false;

    f.open(fileName, std::ios_base::in | std::ios_base::binary);
    if (f.is_open())
    {
        std::ostringstream contents;
        contents << f.rdbuf();
        f.close();
        valid = ParseString(json, contents.str());
    }

    return valid;
}
/*****************************************************************************/
bool JsonReader::ParseString(JsonValue &json, const std::string &data)
{
    char *endptr;
    bool valid = false;

#ifdef USE_WINDOWS_OS
    char *source = _strdup(data.c_str());
#else
    char *source = strdup(data.c_str());
#endif

    JsonReader::ParseStatus status = Parse(source, &endptr, json);
    if (status == JSON_PARSE_OK)
    {
        valid = true;
    }
    free(source);
    return valid;
}
/*****************************************************************************/
JsonReader::ParseStatus JsonReader::Parse(char *s, char **endptr, JsonValue &json)
{
    int pos = -1;
    int prev = -1;
    std::map<int, JsonValue::Tag> tags; // keep a trace of tags (open tag/close tag)
    std::map<int, std::string> keys; // keep a trace of keys inside an object (multiple keys are forbidden)
    std::map<int, JsonValue> nodes; // keep a link of previous nodes

    bool separator = true;
    *endptr = s;

    while (*s)
    {
        JsonValue o;
        while (*s && isspace(*s))
        {
            ++s;
        }

        *endptr = s++;
        switch (**endptr)
        {
            case '\0':
                continue;
                break;
            case '-':
                if (!isdigit(*s) && *s != '.')
                {
                    *endptr = s;
                    return JSON_PARSE_BAD_NUMBER;
                }
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                o = JsonValue(StringToNumber(*endptr, &s));
                if (!IsDelim(*s))
                {
                    *endptr = s;
                    return JSON_PARSE_BAD_NUMBER;
                }
                break;
            case '"':
            {
                std::string temp;
#ifdef JSON_READER_D
                std::cout << "JSON_TAG_STRING" << std::endl;
#endif
                while (*s)
                {
                    //int c = *it = *s;
                    int c = *s;
                    if (c == '\\')
                    {
                        c = *++s;
                        switch (c)
                        {
                            case '\\':
                            case '"':
                            case '/':
                                temp.push_back(c);
                                break;
                            case 'b':
                                temp.push_back('\b');
                                break;
                            case 'f':
                                temp.push_back('\f');
                                break;
                            case 'n':
                                temp.push_back('\n');
                                break;
                            case 'r':
                                temp.push_back('\r');
                                break;
                            case 't':
                                temp.push_back('\t');
                                break;
                            case 'u':
                                // Manage unicode encoding
                                c = 0;
                                for (int i = 0; i < 4; ++i)
                                {
                                    if (!isxdigit(*++s))
                                    {
                                        *endptr = s;
                                        return JSON_PARSE_BAD_STRING;
                                    }
                                    c = c * 16 + CharToInt(*s);
                                }
                                if (c < 0x80)
                                {
                                    //*it = c;
                                    temp.push_back(c);
                                }
                                else if (c < 0x800)
                                {
                                    temp.push_back(0xC0 | (c >> 6));
                                    temp.push_back(0x80 | (c & 0x3F));
                                }
                                else
                                {
                                    temp.push_back(0xE0 | (c >> 12));
                                    temp.push_back(0x80 | ((c >> 6) & 0x3F));
                                    temp.push_back(0x80 | (c & 0x3F));
                                }
                                break;
                            default:
                                *endptr = s;
                                return JSON_PARSE_BAD_STRING;
                        }
                    }
                    else if (iscntrl(c))
                    {
                        *endptr = s;
                        return JSON_PARSE_BAD_STRING;
                    }
                    else if (c == '"')
                    {
                        ++s;
                        break;
                    }
                    else
                    {
                        temp.push_back(c);
                    }
                    s++;
                }

                // Save the string into the JsonValue
                o = JsonValue(temp);

#ifdef JSON_READER_D
                std::cout << "Found string: "  << temp << std::endl;
#endif

                // There must be a ':' after the key or a coma "," after a value
                if (!IsDelim(*s))
                {
                    *endptr = s;
                    return JSON_PARSE_BAD_STRING;
                }
                break;
            }
            case 't':
                for (const char *it = "rue"; *it; ++it, ++s)
                {
                    if (*it != *s)
                    {
                        return JSON_PARSE_BAD_IDENTIFIER;
                    }
                }
                if (!IsDelim(*s))
                {
                    return JSON_PARSE_BAD_IDENTIFIER;
                }

                o = JsonValue(true);
#ifdef JSON_READER_D
                std::cout << "JSON_TAG_BOOL" << std::endl;
#endif
                break;
            case 'f':
                for (const char *it = "alse"; *it; ++it, ++s)
                {
                    if (*it != *s)
                    {
                        return JSON_PARSE_BAD_IDENTIFIER;
                    }
                }
                if (!IsDelim(*s))
                {
                    return JSON_PARSE_BAD_IDENTIFIER;
                }
                o = JsonValue(false);
#ifdef JSON_READER_D
                std::cout << "JSON_TAG_BOOL" << std::endl;
#endif

                break;
            case 'n':
                for (const char *it = "ull"; *it; ++it, ++s)
                {
                    if (*it != *s)
                    {
                        return JSON_PARSE_BAD_IDENTIFIER;
                    }
                }
                if (!IsDelim(*s))
                {
                    return JSON_PARSE_BAD_IDENTIFIER;
                }
                o.SetNull();
#ifdef JSON_READER_D
                std::cout << "JSON_TAG_BOOL" << std::endl;
#endif

                break;
            case ']':
                if (pos == -1)
                {
                    return JSON_PARSE_STACK_UNDERFLOW;
                }
                if (tags[pos] != JsonValue::ARRAY)
                {
                    return JSON_PARSE_MISMATCH_BRACKET;
                }

                if (pos > 0)
                {
                    // We have finished with this object, add it to the previous node and delete it
                    prev = pos - 1;
                    if (tags[prev] == JsonValue::OBJECT)
                    {
                        // if previous node was an object, then we create a new object
                        nodes[prev].GetObject().AddValue(keys[prev], nodes[pos]);
                    }
                    else
                    {
                        // Object in an array? ==> no tag
                        nodes[prev].GetArray().AddValue(nodes[pos]);
                    }

                    nodes.erase(pos);
                    pos--;
                    keys[pos] = ""; // key has been used to create the array
                    separator = false;
                }
                else
                {
                    // End of document
                    json = nodes[0];
                    *endptr = s;
                    return JSON_PARSE_OK;
                }
                continue;

#ifdef JSON_READER_D
                std::cout << "TAG_ARRAY_END" << std::endl;
#endif

                break;
            case '}':
                if (pos == -1)
                {
                    return JSON_PARSE_STACK_UNDERFLOW;
                }
                if (tags[pos] != JsonValue::OBJECT)
                {
                    return JSON_PARSE_MISMATCH_BRACKET;
                }
                if (keys[pos] != "")
                {
                    return JSON_PARSE_UNEXPECTED_CHARACTER;
                }

                if (pos > 0)
                {
                    // We have finished with this object, add it to the previous node and delete it
                    prev = pos - 1;
                    if (tags[prev] == JsonValue::OBJECT)
                    {
                        // if previous node was an object, then we create a new object
                        nodes[prev].GetObject().AddValue(keys[prev], nodes[pos]);
                    }
                    else
                    {
                        // Object in an array? ==> no tag
                        nodes[prev].GetArray().AddValue(nodes[pos]);
                    }

                    nodes.erase(pos);
                    pos--;
                    keys[pos] = ""; // key has been used to create the object
                    separator = false;
                }
                else
                {
                    // End of document
                    json = nodes[0];
                    *endptr = s;
                    return JSON_PARSE_OK;
                }
                continue;

#ifdef JSON_READER_D
                std::cout << "TAG_OBJECT_END" << std::endl;
#endif
                break;
            case '[':

#ifdef JSON_READER_D
                std::cout << "TAG_ARRAY_BEGIN" << std::endl;
#endif
                ++pos;
                nodes[pos] = JsonValue(JsonArray());
                tags[pos] = JsonValue::ARRAY;
                keys[pos] = "";
                separator = true;
                continue;
            case '{':

#ifdef JSON_READER_D
                std::cout << "TAG_OBJECT_BEGIN" << std::endl;
#endif
                ++pos;
                nodes[pos] = JsonValue(JsonObject());
                tags[pos] = JsonValue::OBJECT;
                keys[pos] = "";
                separator = true;
                continue;
            case ':':
                if (separator || keys[pos] == "")
                {
                    return JSON_PARSE_UNEXPECTED_CHARACTER;
                }
                separator = true;
                continue;
            case ',':
                if (separator || keys[pos] != "")
                {
                    return JSON_PARSE_UNEXPECTED_CHARACTER;
                }
                separator = true;
                continue;
            default:
                return JSON_PARSE_UNEXPECTED_CHARACTER;
        }
        separator = false;

        // We are here because we are in an object
        if (tags[pos] == JsonValue::OBJECT)
        {
            if (keys[pos] == "")
            {
                // Key found!
                if (o.GetTag() != JsonValue::STRING)
                {
                    return JSON_PARSE_UNQUOTED_KEY;
                }
                keys[pos] = o.GetString();
                continue; // Let's continue to find the value ...
            }

            // Use our own type management, event if the RTTI thing would also work...
            if (nodes[pos].IsObject())
            {
                nodes[pos].GetObject().AddValue(keys[pos], o);
                keys[pos] = "";
            }
            else
            {
                return JSON_PARSE_ALLOC_ERROR;
            }
        }
        else
        {
            // We are in an Array
            // Use our own type management, event if the RTTI thing would also work...
            if (nodes[pos].IsArray())
            {
                nodes[pos].GetArray().AddValue(o);
                keys[pos] = "";
            }
            else
            {
                return JSON_PARSE_ALLOC_ERROR;
            }
        }
    }
    return JSON_PARSE_BREAKING_BAD;
}
/*****************************************************************************/
JsonValue JsonReader::StringToNumber(char *s, char **endptr)
{
    char ch = *s;
    if (ch == '+' || ch == '-')
    {
        ++s;
    }

    double result = 0;
    bool doubleValue = false;
    JsonValue retVal;

    while (isdigit(*s))
    {
        result = (result * 10) + (*s++ - '0');
    }

    if (*s == '.')
    {
        ++s;
        doubleValue = true;

        double fraction = 1;
        while (isdigit(*s))
        {
            fraction *= 0.1;
            result += (*s++ - '0') * fraction;
        }
    }

    if (*s == 'e' || *s == 'E')
    {
        ++s;
        doubleValue = true;

        double base = 10;
        if (*s == '+')
        {
            ++s;
        }
        else if (*s == '-')
        {
            ++s;
            base = 0.1;
        }

        int exponent = 0;
        while (isdigit(*s))
        {
            exponent = (exponent * 10) + (*s++ - '0');
        }

        double power = 1;
        for (; exponent; exponent >>= 1, base *= base)
        {
            if (exponent & 1)
            {
                power *= base;
            }
        }

        result *= power;
    }

    *endptr = s;

    result = (ch == '-') ? -result : result;

    if (doubleValue)
    {
        retVal = result;
    }
    else
    {
        retVal = static_cast<std::int32_t>(result);
    }

    return retVal;
}


//=============================================================================
// End of file JsonReader.cpp
//=============================================================================
