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

#include "JSValue.h"

/*****************************************************************************/
class IJsonNode
{
public:
    virtual ~IJsonNode() {}

    virtual std::string ToString() = 0;
};
/*****************************************************************************/
class JsonValue : public JSValue, public IJsonNode
{

public:

    JsonValue(std::int32_t value) : JSValue(value) {}
    JsonValue(double value) : JSValue(value) {}
    JsonValue(const char *value) : JSValue(value) {}
    JsonValue(const std::string &value) : JSValue(value) {}
    JsonValue(bool value) : JSValue(value) {}
    JsonValue(const JSValue &value) : JSValue(value) {}
    JsonValue() : JSValue() {}

    std::string ToString();
};

#endif // JSONVALUE_H

//=============================================================================
// End of file JsonValue.h
//=============================================================================
