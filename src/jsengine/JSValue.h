/*=============================================================================
 * TarotClub - JSValue.h
 *=============================================================================
 * JavaScript value class
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

#ifndef JSVALUE_H
#define JSVALUE_H

#include <string>
#include <cstdint>

/*****************************************************************************/
/**
 * @brief The JSValue class
 * The currently supported values are:
 *  \li a double
 *  \li a string
 *  \li an integer
 *  \li a boolean
 *
 * null is not supported. Array and Object are managed with dedicated classes.
 */
class JSValue
{
public:
    enum ValueType
    {
        INVALID,
        INTEGER,
        DOUBLE,
        BOOLEAN,
        STRING
    };

    JSValue(std::int32_t value);
    JSValue(double value);
    JSValue(const char *value);
    JSValue(const std::string &value);
    JSValue(bool value);
    JSValue(const JSValue &value);
    JSValue(); // default constructor creates an invalid value!
    ~JSValue();

    JSValue &operator = (JSValue const &rhs);

    bool IsValid() { return mType != INVALID; }
    ValueType GetType() { return mType; }

    std::int32_t    GetInteger(){ return mIntegerValue; }
    double          GetDouble() { return mDoubleValue; }
    bool            GetBool()   { return mBoolValue; }
    std::string     GetString() { return mStringValue; }

private:
    ValueType mType;
    std::int32_t mIntegerValue;
    double mDoubleValue;
    std::string mStringValue;
    bool mBoolValue;
};

#endif // JSVALUE_H

//=============================================================================
// End of file JSValue.h
//=============================================================================
