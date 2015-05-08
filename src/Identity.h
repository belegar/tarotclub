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
struct Identity
{
public:
    static const std::uint8_t cGenderInvalid    = 0U; ///< Invalid player or identity
    static const std::uint8_t cGenderMale       = 1U; ///< Human player
    static const std::uint8_t cGenderFemale     = 2U; ///< Human player
    static const std::uint8_t cGenderRobot      = 3U; ///< AI bot attached to a user account
    static const std::uint8_t cGenderDummy      = 4U; ///< Dummy player is to replace missing player

    std::string     nickname;
    std::string     avatar;     ///< Path to the avatar image (local or network path)
    std::uint8_t    gender;
    std::string     username;   ///< Empty username means no account

    Identity()
        : gender(cGenderInvalid)
    {

    }

    // operator overload to easily serialize parameters
    friend ByteStreamWriter &operator<<(ByteStreamWriter &out, const Identity &ident)
    {
        out << ident.nickname
            << ident.avatar
            << ident.gender
            << ident.username;
        return out;
    }

    friend ByteStreamReader &operator>>(ByteStreamReader &in, Identity &ident)
    {
        in >> ident.nickname;
        in >> ident.avatar;
        in >> ident.gender;
        in >> ident.username;

        return in;
    }

};

#endif // IDENTITY_H

//=============================================================================
// End of file Identity.h
//=============================================================================
