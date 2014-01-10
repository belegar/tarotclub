/*=============================================================================
 * TarotClub - Common.h
 *=============================================================================
 * Common Tarot related classes
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

#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <cstdint>
#include <vector>
#include "ByteStreamReader.h"
#include "ByteStreamWriter.h"

/*****************************************************************************/
class Place
{
public:
    static const std::string STR_SOUTH;
    static const std::string STR_EAST;
    static const std::string STR_NORTH;
    static const std::string STR_WEST;

    static const std::uint8_t SOUTH;
    static const std::uint8_t EAST;
    static const std::uint8_t NORTH;
    static const std::uint8_t WEST;
    static const std::uint8_t FIFTH;
    static const std::uint8_t NOWHERE;

    // Constructors
    Place();
    Place(int p);
    Place(std::uint8_t p);

    std::string ToString() const;
    std::uint8_t Value();

    Place& operator = (Place const &rhs)
    {
        mPlace = rhs.mPlace;
        return *this;
    }

    inline bool operator == (const Place& rhs) const { return (this->mPlace == rhs.mPlace); }
    inline bool operator != (const Place& rhs) const { return (this->mPlace != rhs.mPlace); }
    inline bool operator <  (const Place& rhs) const { return (this->mPlace < rhs.mPlace);  }
    inline bool operator >  (const Place& rhs) const { return (this->mPlace > rhs.mPlace); }

    friend ByteStreamWriter &operator<<(ByteStreamWriter &out, const Place &p)
    {
        out << p.mPlace;
        return out;
    }

    friend ByteStreamReader &operator>>(ByteStreamReader &s, Place &l)
    {
        s >> l.mPlace;
        return s;
    }

private:
    std::uint8_t mPlace;
    static std::vector<std::string> mStrings;

    static std::vector<std::string> Initialize();
};
/*****************************************************************************/
class Contract
{
public:
    static const std::string STR_PASS;
    static const std::string STR_TAKE;
    static const std::string STR_GUARD;
    static const std::string STR_GUARD_WITHOUT;
    static const std::string STR_GUARD_AGAINST;

    static const std::uint8_t PASS;
    static const std::uint8_t TAKE;
    static const std::uint8_t GUARD;
    static const std::uint8_t GUARD_WITHOUT;
    static const std::uint8_t GUARD_AGAINST;

    // Constructors
    Contract();
    Contract(int c);
    Contract(std::uint8_t c);

    std::string ToString() const;
    std::uint8_t Value();

    Contract& operator = (Contract const &rhs)
    {
        mContract = rhs.mContract;
        return *this;
    }

    inline bool operator == (const Contract& rhs) const { return (this->mContract == rhs.mContract);}
    inline bool operator != (const Contract& rhs) const { return (this->mContract != rhs.mContract);}
    inline bool operator <  (const Contract& rhs) const { return (this->mContract < rhs.mContract); }
    inline bool operator <= (const Contract& rhs) const { return (this->mContract <= rhs.mContract);}
    inline bool operator >  (const Contract& rhs) const { return (this->mContract > rhs.mContract); }
    inline bool operator >= (const Contract& rhs) const { return (this->mContract >=rhs.mContract); }

    friend ByteStreamWriter &operator<<(ByteStreamWriter &out, const Contract &p)
    {
        out << p.mContract;
        return out;
    }

    friend ByteStreamReader &operator>>(ByteStreamReader &s, Contract &l)
    {
        s >> l.mContract;
        return s;
    }

private:
    std::uint8_t mContract;
    static std::vector<std::string> mStrings;

    static std::vector<std::string> Initialize();
};

#endif // COMMON_H

//=============================================================================
// End of file Common.h
//=============================================================================
