/*=============================================================================
 * TarotClub - Identity.h
 *=============================================================================
 * Identity of one player
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


#ifndef IDENTITY_H
#define IDENTITY_H

#include <string>
#include <cstdint>
#include "ByteStreamReader.h"
#include "ByteStreamWriter.h"

/*****************************************************************************/
class Identity
{
public:
    enum Gender
    {
        MALE,
        FEMALE
    };

    Identity()
    {
        sex = MALE;
    }

    std::string name;
    std::string quote;
    std::string avatar;  // path to the avatar image (local or network path)
    Gender      sex;

    // operator overload to easily serialize parameters
    friend ByteStreamWriter &operator<<(ByteStreamWriter &out, Identity &ident)
    {
        out << ident.name
            << ident.avatar
            << ident.quote
            << (std::uint8_t)ident.sex;
        return out;
    }

    friend ByteStreamReader &operator>>(ByteStreamReader &in, Identity &ident)
    {
        std::uint8_t var8;

        in >> ident.name;
        in >> ident.avatar;
        in >> ident.quote;
        in >> var8;
        ident.sex = (Gender)var8;

        return in;
    }

};

#endif // IDENTITY_H

//=============================================================================
// End of file Identity.h
//=============================================================================
