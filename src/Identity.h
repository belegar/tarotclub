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

/*****************************************************************************/
struct Identity
{
public:
    static const std::uint8_t cGenderInvalid    = 0U; ///< Invalid player or identity
    static const std::uint8_t cGenderMale       = 1U; ///< Human player
    static const std::uint8_t cGenderFemale     = 2U; ///< Human player
    static const std::uint8_t cGenderRobot      = 3U; ///< AI bot attached to a user account
    static const std::uint8_t cGenderDummy      = 4U; ///< Dummy player is to replace missing player

    static const std::string cStrInvalid;
    static const std::string cStrMale;
    static const std::string cStrFemale;
    static const std::string cStrRobot;
    static const std::string cStrDummy;

    std::string     nickname;
    std::string     avatar;     ///< Path to the avatar image (local or network path)
    std::uint8_t    gender;

    Identity()
        : nickname("John Doe")
        , gender(cGenderInvalid)
    {

    }

    Identity(const std::string &n, const std::string &a, std::uint8_t g)
        : nickname(n)
        , avatar(a)
        , gender(g)
    {

    }

    Identity(Identity const&) = default;
    ~Identity() = default;

    Identity& operator=(Identity other)
    {
        nickname = other.nickname;
        avatar = other.nickname;
        gender = other.gender;
        return *this;
    }

    std::string GenderToString()
    {
        std::string txt = cStrInvalid;
        switch(gender)
        {
        case cGenderInvalid:
            txt = "Invalid";
            break;
        case cGenderMale:
            txt = cStrMale;
            break;
        case cGenderFemale:
            txt = "Female";
            break;
        case cGenderRobot:
            txt = "Robot";
            break;
        case cGenderDummy:
            txt = "Dummy";
            break;
        }
        return txt;
    }

    bool GenderFromString(const std::string &txt)
    {
        bool valid = true;
        if (txt == cStrInvalid)
        {
            gender = cGenderInvalid;
        }
        else if (txt == cStrMale)
        {
            gender = cGenderMale;
        }
        else if (txt == cStrFemale)
        {
            gender = cGenderFemale;
        }
        else if (txt == cStrRobot)
        {
            gender = cGenderRobot;
        }
        else if (txt == cStrDummy)
        {
            gender = cGenderDummy;
        }
        else
        {
            gender = cGenderInvalid;
            valid = false;
        }
        return valid;
    }
};

#endif // IDENTITY_H

//=============================================================================
// End of file Identity.h
//=============================================================================
