/*=============================================================================
 * TarotClub - Value.cpp
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

#include "Value.h"

/*****************************************************************************/
Value::Value(std::int32_t value)
    : mType(INTEGER)
    , mIntegerValue(value)
    , mDoubleValue(0.0F)
    , mBoolValue(false)
{

}
/*****************************************************************************/
Value::Value(double value)
    : mType(DOUBLE)
    , mIntegerValue(0)
    , mDoubleValue(value)
    , mBoolValue(false)
{

}
/*****************************************************************************/
Value::Value(const char * value)
    : mType(STRING)
    , mIntegerValue(0)
    , mDoubleValue(0.0F)
    , mStringValue(value)
    , mBoolValue(false)
{

}
/*****************************************************************************/
Value::Value(const std::string &value)
    : mType(STRING)
    , mIntegerValue(0)
    , mDoubleValue(0.0F)
    , mStringValue(value)
    , mBoolValue(false)
{

}
/*****************************************************************************/
Value::Value(bool value)
    : mType(BOOLEAN)
    , mIntegerValue(0)
    , mDoubleValue(0.0F)
    , mBoolValue(value)
{

}
/*****************************************************************************/
Value::Value(const Value &value)
{
    *this = value;
}
/*****************************************************************************/
Value::Value()
    : mType(INVALID)
    , mIntegerValue(0)
    , mDoubleValue(0.0F)
    , mBoolValue(false)
{

}
/*****************************************************************************/
Value::~Value()
{

}
/*****************************************************************************/
Value &Value::operator =(const Value &rhs)
{
    mType = rhs.mType;
    mIntegerValue = rhs.mIntegerValue;
    mDoubleValue = rhs.mDoubleValue;
    mStringValue = rhs.mStringValue;
    mBoolValue = rhs.mBoolValue;
    return *this;
}

//=============================================================================
// End of file Value.cpp
//=============================================================================
