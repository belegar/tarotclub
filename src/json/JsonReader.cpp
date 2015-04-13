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
JsonReader::JsonReader()
{
    mValid = false;
    mRootNode = nullptr;
}
/*****************************************************************************/
JsonReader::~JsonReader()
{
    Close();
}
/*****************************************************************************/
/**
 * @brief Open a JSON file for parsing
 *
 * After opening the JSON file, the context is ready for requests.
 *
 * @param fileName to open
 * @return true if the file has been parsed successfully
 */
bool JsonReader::Open(const std::string &fileName)
{
    std::ifstream f;

    f.open(fileName, std::ios_base::in | std::ios_base::binary);
    if (f.is_open())
    {
        std::ostringstream contents;
        contents << f.rdbuf();
        f.close();
        mValid = ParseString(contents.str());
    }
    else
    {
        mValid = false;
    }

    return mValid;
}
/*****************************************************************************/
/**
 * @brief Close
 *
 * Cleans the context and free memory
 *
 */
void JsonReader::Close()
{
    mValid = false;
    if (mRootNode != nullptr)
    {
        delete mRootNode;
    }
    mRootNode = nullptr;
}
/*****************************************************************************/
bool JsonReader::ParseString(const std::string &data)
{
    char *endptr;

#ifdef USE_WINDOWS_OS
    char *source = _strdup(data.c_str());
#else
    char *source = strdup(data.c_str());
#endif
    if (Parse(source, &endptr) == JSON_PARSE_OK)
    {
        mValid = true;
    }
    else
    {
        mValid = false;
    }
    free(source);
    return mValid;
}
/*****************************************************************************/
bool JsonReader::GetValue(const std::string &nodePath, std::int32_t &value)
{
    return JsonValue::FromNode(FindNode(nodePath), value);
}
/*****************************************************************************/
bool JsonReader::GetValue(const std::string &nodePath, std::uint32_t &value)
{
    return JsonValue::FromNode(FindNode(nodePath), value);
}
/*****************************************************************************/
bool JsonReader::GetValue(const std::string &nodePath, std::string &value)
{
    return JsonValue::FromNode(FindNode(nodePath), value);
}
/*****************************************************************************/
bool JsonReader::GetValue(const std::string &nodePath, bool &value)
{
    return JsonValue::FromNode(FindNode(nodePath), value);
}
/*****************************************************************************/
bool JsonReader::GetValue(const std::string &nodePath, double &value)
{
    return JsonValue::FromNode(FindNode(nodePath), value);
}
/*****************************************************************************/
std::vector<JsonValue> JsonReader::GetArray(const std::string &nodePath)
{
    std::vector<JsonValue> retval;
    IJsonNode *node = FindNode(nodePath);

    if (node != NULL)
    {
        if (node->GetTag() == IJsonNode::ARRAY)
        {
            // we are arrived to the array, get it
            JsonArray *array = dynamic_cast<JsonArray *>(node);

            if (array != NULL)
            {
                // Fill the array with the values
                for (std::uint32_t j = 0U; j < array->GetSize(); j++)
                {
                    IJsonNode *subNode = array->GetEntry(j);
                    if (subNode->GetTag() == IJsonNode::OBJECT)
                    {
                        retval.push_back(dynamic_cast<JsonObject *>(subNode));
                    }
                    else if (subNode->GetTag() == IJsonNode::ARRAY)
                    {
                        retval.push_back(dynamic_cast<JsonArray *>(subNode));
                    }
                    else
                    {
                        JsonValue *jsonValue = dynamic_cast<JsonValue *>(subNode);
                        if (jsonValue->IsValid())
                        {
                            retval.push_back(*jsonValue);
                        }
                    }
                }
            }
        }
    }

    return retval;
}
/*****************************************************************************/
IJsonNode *JsonReader::FindNode(const std::string &nodePath)
{
    IJsonNode *node = NULL;

    std::vector<std::string> path = Split(nodePath);

    if (mValid)
    {
        node = mRootNode;
        for (std::uint32_t i = 0U; i < path.size(); i++)
        {
            std::string key = path[i];
            if (node != nullptr)
            {
                if (node->HasNode(key))
                {
                    node = node->GetNode(key);
                }
            }
            else
            {
                // problem, exit loop
                break;
            }
        }
    }
    return node;
}
/*****************************************************************************/
JsonReader::ParseStatus JsonReader::Parse(char *s, char **endptr)
{
    int pos = -1;
    std::map<int, IJsonNode::Tag> tags; // keep a trace of tags (open tag/close tag)
    std::map<int, std::string> keys; // keep a trace of keys inside an object (multiple keys are forbidden)
    std::map<int, IJsonNode *> nodes;

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
                                //*it = c;
                                temp.push_back(c);
                                break;
                            case 'b':
                                //*it = '\b';
                                temp.push_back('\b');
                                break;
                            case 'f':
                                //*it = '\f';
                                temp.push_back('\f');
                                break;
                            case 'n':
                                //*it = '\n';
                                temp.push_back('\n');
                                break;
                            case 'r':
                                //*it = '\r';
                                temp.push_back('\r');
                                break;
                            case 't':
                                //*it = '\t';
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
                                    //*it++ = 0xC0 | (c >> 6);
                                    //*it = 0x80 | (c & 0x3F);
                                    temp.push_back(0xC0 | (c >> 6));
                                    temp.push_back(0x80 | (c & 0x3F));
                                }
                                else
                                {
                                    //*it++ = 0xE0 | (c >> 12);
                                    //*it++ = 0x80 | ((c >> 6) & 0x3F);
                                    //*it = 0x80 | (c & 0x3F);
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
                if (tags[pos] != IJsonNode::ARRAY)
                {
                    return JSON_PARSE_MISMATCH_BRACKET;
                }
                // We have finished with this object, we do not need to keep trace of the pointer
                nodes[pos] = NULL;
                pos--;
                keys[pos] = ""; // key has been used to create the array
                separator = false;
                if (pos == -1)
                {
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
                if (tags[pos] != IJsonNode::OBJECT)
                {
                    return JSON_PARSE_MISMATCH_BRACKET;
                }
                if (keys[pos] != "")
                {
                    return JSON_PARSE_UNEXPECTED_CHARACTER;
                }
                // We have finished with this object, we do not need to keep trace of the pointer
                nodes[pos] = NULL;
                pos--;
                keys[pos] = ""; // key has been used to create the object
                separator = false;
                if (pos == -1)
                {
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
                if (pos == 0)
                {
                    // We create the root node of the document
                    mRootNode = new JsonArray(0U);
                    nodes[pos] = mRootNode;
                }
                else if (tags[pos - 1] == IJsonNode::OBJECT)
                {
                    // if previous node was an object, then we create a new array
                    JsonObject *obj = dynamic_cast<JsonObject *>(nodes[pos - 1]);
                    nodes[pos] = obj->CreateArrayPair(keys[pos - 1]);
                }
                else
                {
                    // array in an array?
                    JsonArray *array = dynamic_cast<JsonArray *>(nodes[pos - 1]);
                    nodes[pos] = array->CreateArray();
                }

                tags[pos] = IJsonNode::ARRAY;
                keys[pos] = "";
                separator = true;
                continue;
            case '{':

#ifdef JSON_READER_D
                std::cout << "TAG_OBJECT_BEGIN" << std::endl;
#endif
                ++pos;
                if (pos == 0)
                {
                    // We create the root node of the document
                    mRootNode = new JsonObject(0U);
                    nodes[pos] = mRootNode;
                }
                else if (tags[pos - 1] == IJsonNode::OBJECT)
                {
                    // if previous node was an object, then we create a new object
                    JsonObject *obj = dynamic_cast<JsonObject *>(nodes[pos - 1]);
                    nodes[pos] = obj->CreateObjectPair(keys[pos - 1]);
                }
                else
                {
                    // Object in an array? ==> no tag
                    JsonArray *array = dynamic_cast<JsonArray *>(nodes[pos - 1]);
                    nodes[pos] = array->CreateObject();
                }

                //tails[pos] = nullptr;
                tags[pos] = IJsonNode::OBJECT;
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
        if (tags[pos] == IJsonNode::OBJECT)
        {
            if (keys[pos] == "")
            {
                // Key found!
                if (o.GetTag() != IJsonNode::STRING)
                {
                    return JSON_PARSE_UNQUOTED_KEY;
                }
                keys[pos] = o.GetString();
                continue; // Let's continue to find the value ...
            }

            // Use our own type management, event if the RTTI thing would also work...
            if (nodes[pos]->GetTag() == IJsonNode::OBJECT)
            {
                JsonObject *obj = dynamic_cast<JsonObject *>(nodes[pos]);
                obj->CreateValuePair(keys[pos], o);
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
            if (nodes[pos]->GetTag() == IJsonNode::ARRAY)
            {
                JsonArray *array = dynamic_cast<JsonArray *>(nodes[pos]);
                array->CreateValue(o);
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
/*****************************************************************************/
std::vector<std::string> JsonReader::Split(const std::string &obj)
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
// End of file JsonReader.cpp
//=============================================================================
