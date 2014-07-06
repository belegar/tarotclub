/*=============================================================================
 * TarotClub - JsonValue.cpp
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

#include "JsonValue.h"
#include <sstream>

/*****************************************************************************/
std::string JsonValue::ToString()
{
    std::string text;
    std::stringstream ss;

    if (GetType() == STRING)
    {
        text += "\"" + GetString() + "\"";
    }
    else if (GetType() == INTEGER)
    {
        ss << GetInteger();
        text += ss.str();
    }
    else if (GetType() == BOOLEAN)
    {
        if (GetBool())
        {
            text = "true";
        }
        else
        {
            text = "false";
        }
    }
    else if (GetType() == DOUBLE)
    {
        ss << GetDouble();
        text += ss.str();
    }

    return text;
}

//=============================================================================
// End of file JsonValue.cpp
//=============================================================================
