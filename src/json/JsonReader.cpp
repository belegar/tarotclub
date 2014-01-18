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

#include "JsonReader.h"

/*****************************************************************************/
JsonReader::JsonReader()
{
    mCtx = NULL;
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
    bool retval = false;
    f.open(fileName, std::ios_base::in | std::ios_base::binary);

    if (f.is_open())
    {
        mCtx = duk_create_heap_default();
        std::ostringstream contents;
        contents << f.rdbuf();
        f.close();

        duk_push_lstring(mCtx, contents.str().c_str(), contents.str().size());

        int rc = duk_safe_call(mCtx, JsonReader::WrappedJsonDecode, 1 /*nargs*/, 1 /*nret*/, DUK_INVALID_INDEX);
        if (rc != DUK_EXEC_SUCCESS)
        {
            PrintError();
            mValid = false;
            retval = false;
        }
        else
        {
            mValid = true;
            retval = true;
        }
    }

    return retval;
}
/*****************************************************************************/
/**
 * @brief GetValue
 *
 * Gets a JSON value by specifying the key.
 * The value returned must be a string format, if not, false is returned
 *
 * @param key
 * @param value
 * @return
 */
bool JsonReader::GetValue(const std::string &key, std::string &value)
{
    if (!mValid)
    {
        return false;
    }

    // Push argument into the stack (JSON key)
    duk_push_lstring(mCtx, key.c_str(), key.size());

    int rc = duk_safe_call(mCtx, JsonReader::WrappedJsonGetValue, 1 /*nargs*/, 1 /*nret*/, DUK_INVALID_INDEX);
    if (rc != DUK_EXEC_SUCCESS)
    {
        PrintError();
        return false;
    }
    else
    {
        if (duk_check_type(mCtx, -1, DUK_TYPE_STRING))
        {
            value = duk_get_string(mCtx, -1);
            duk_pop(mCtx);
        }
        return true;
    }
}
/*****************************************************************************/
bool JsonReader::GetValue(const std::string &obj, const std::string &key, std::string &value)
{
    if (!mValid)
    {
        return false;
    }

    // Push arguments into the stack (JSON object and key)
    duk_push_lstring(mCtx, obj.c_str(), obj.size());
    duk_push_lstring(mCtx, key.c_str(), key.size());

    int rc = duk_safe_call(mCtx, JsonReader::WrappedJsonGetObjectValue, 2 /*nargs*/, 1 /*nret*/, DUK_INVALID_INDEX);
    if (rc != DUK_EXEC_SUCCESS)
    {
        PrintError();
        return false;
    }
    else
    {
        if (duk_check_type(mCtx, -1, DUK_TYPE_STRING))
        {
            value = duk_get_string(mCtx, -1);
            duk_pop(mCtx);
        }
        return true;
    }

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
    if (mCtx)
    {
        duk_destroy_heap(mCtx);
    }
    mValid = false;
}
/*****************************************************************************/
/**
 * @brief WrappedJsonDecode
 *
 * Decode a JSON string, throw an error if parsing problem
 *
 * @param ctx
 * @return
 */
int JsonReader::WrappedJsonDecode(duk_context *ctx)
{
    duk_json_decode(ctx, -1);
    return 1; // return parsed json object
}
/*****************************************************************************/
/**
 * @brief WrappedJsonGetValue
 *
 * Key is pushed into the stack
 *
 * @param ctx
 * @return
 */
int JsonReader::WrappedJsonGetValue(duk_context *ctx)
{
    std::string key = duk_get_string(ctx, -1);
    duk_pop(ctx);

    if(duk_has_prop_string(ctx, -1, key.c_str()))
    {
        duk_get_prop_string(ctx, -1, key.c_str());
        // leave the answer into the stack to retreive it by the caller
        // so, we have one return value
        return 1;
    }
    return 0; // no return values
}
/*****************************************************************************/
/**
 * @brief WrappedJsonGetObjectValue
 *
 * Gets a property in an object value
 *
 * @param ctx
 * @return
 */
int JsonReader::WrappedJsonGetObjectValue(duk_context *ctx)
{
    std::string obj = duk_get_string(ctx, -2); // [ json "obj" "key" ]
    std::string key = duk_get_string(ctx, -1);
    duk_pop(ctx); // [ json "obj" ]

    duk_get_prop(ctx, -2);  // [ json obj ] object name has been replaced by the real object

    if (duk_has_prop_string(ctx, -1, key.c_str()))
    {
        duk_get_prop_string(ctx, -1, key.c_str());
        // leave the answer into the stack to retreive it by the caller
        // so, we have one return value
        return 1;
    }
    return 0; // no return values
}
/*****************************************************************************/
/**
 * @brief PrintError
 *
 * Print and pop error.
 *
 */
void JsonReader::PrintError()
{
    if (duk_is_object(mCtx, -1) && duk_has_prop_string(mCtx, -1, "stack"))
    {
        /* FIXME: print error objects specially */
        /* FIXME: pcall the string coercion */
        duk_get_prop_string(mCtx, -1, "stack");
        if (duk_is_string(mCtx, -1))
        {
            std::cout << duk_get_string(mCtx, -1) << std::endl;
            duk_pop_2(mCtx);
            return;
        }
        else
        {
            duk_pop(mCtx);
        }
    }
    duk_to_string(mCtx, -1);
    std::cout << duk_get_string(mCtx, -1) << std::endl;
    duk_pop(mCtx);
}
/*****************************************************************************/
/**
 * @brief PrintTop
 *
 * Prints the top value information of the stack
 */
void JsonReader::PrintTop()
{
    std::cout << "Stack size: " << duk_get_top(mCtx) << std::endl;
    std::cout << "Top type is: " << duk_get_type(mCtx, -1) << std::endl;
}

//=============================================================================
// End of file JsonReader.cpp
//=============================================================================
