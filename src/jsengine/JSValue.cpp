/*=============================================================================
 * TarotClub - JSValue.cpp
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

#include "JSValue.h"

/*****************************************************************************/
JSValue::JSValue(std::int32_t value)
    : mType(INTEGER)
    , mIntegerValue(value)
{

}
/*****************************************************************************/
JSValue::JSValue(double value)
    : mType(DOUBLE)
    , mDoubleValue(value)
{

}
/*****************************************************************************/
JSValue::JSValue(const char * value)
    : mType(STRING)
    , mStringValue(value)
{

}
/*****************************************************************************/
JSValue::JSValue(const std::string &value)
    : mType(STRING)
    , mStringValue(value)
{

}
/*****************************************************************************/
JSValue::JSValue(bool value)
    : mType(BOOLEAN)
    , mBoolValue(value)
{

}
/*****************************************************************************/
JSValue::JSValue(const JSValue &value)
{
    *this = value;
}
/*****************************************************************************/
JSValue::JSValue()
{
    mType = INVALID;
}
/*****************************************************************************/
JSValue::~JSValue()
{

}
/*****************************************************************************/
JSValue &JSValue::operator =(const JSValue &rhs)
{
    mType = rhs.mType;
    mIntegerValue = rhs.mIntegerValue;
    mDoubleValue = rhs.mDoubleValue;
    mStringValue = rhs.mStringValue;
    mBoolValue = rhs.mBoolValue;
    return *this;
}

//=============================================================================
// End of file JSValue.cpp
//=============================================================================
