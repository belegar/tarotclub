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
#include "duktape.h"
#include "JsonValue.h"

/*****************************************************************************/
class JsonReader
{

public:
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
    duk_context *mCtx;
    bool mValid;

    // Static function to be run in protected call
    static int WrappedJsonDecode(duk_context *ctx);
    static int WrappedJsonGetValue(duk_context *ctx);

    JsonValue GetJsonValue(const std::string &obj, const std::string &key, JsonValue::ValueType type);
    void PrintError();
    void PrintTop();
};

#endif // JSON_READER_H

//=============================================================================
// End of file JsonReader.h
//=============================================================================
