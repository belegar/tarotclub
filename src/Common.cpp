/*=============================================================================
 * TarotClub - Common.cpp
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

#include "Common.h"
#include "Log.h"

/*****************************************************************************/
const std::string Place::STR_SOUTH  = "South";
const std::string Place::STR_EAST   = "East";
const std::string Place::STR_NORTH  = "North";
const std::string Place::STR_WEST   = "West";

const std::uint8_t Place::SOUTH     = 0;
const std::uint8_t Place::EAST      = 1;
const std::uint8_t Place::NORTH     = 2;
const std::uint8_t Place::WEST      = 3;
const std::uint8_t Place::FIFTH     = 4;
const std::uint8_t Place::NOWHERE   = 5;

std::vector<std::string> Place::mStrings = Place::Initialize();
/*****************************************************************************/

const std::string Contract::STR_PASS           = "Pass";
const std::string Contract::STR_TAKE           = "Take";
const std::string Contract::STR_GUARD          = "Guard";
const std::string Contract::STR_GUARD_WITHOUT  = "Guard without";
const std::string Contract::STR_GUARD_AGAINST  = "Guard against";

const std::uint8_t Contract::PASS          = 0;
const std::uint8_t Contract::TAKE          = 1;
const std::uint8_t Contract::GUARD         = 2;
const std::uint8_t Contract::GUARD_WITHOUT = 3;
const std::uint8_t Contract::GUARD_AGAINST = 4;

std::vector<std::string> Contract::mStrings = Contract::Initialize();
/*****************************************************************************/
Place::Place()
    : mPlace(NOWHERE)
{

}
/*****************************************************************************/
Place::Place(std::uint32_t p)
{
    if (p > NOWHERE)
    {
        TLogError("Invalid Place value");
        mPlace = SOUTH;
    }
    else
    {
        mPlace = static_cast<std::uint8_t>(p);
    }
}
/*****************************************************************************/
Place::Place(std::uint8_t p)
{
    if (p > NOWHERE)
    {
        TLogError("Invalid Place value");
        mPlace = SOUTH;
    }
    else
    {
        mPlace = p;
    }
}
/*****************************************************************************/
Place::Place(std::string p)
{
    mPlace = NOWHERE;
    for (std::uint32_t i = 0U; i < mStrings.size(); i++)
    {
        if (p == mStrings[i])
        {
            mPlace = static_cast<std::uint8_t>(i);
        }
    }
}
/*****************************************************************************/
std::string Place::ToString() const
{
    return mStrings[mPlace];
}
/*****************************************************************************/
std::uint8_t Place::Value()
{
    return mPlace;
}
/*****************************************************************************/
std::vector<std::string> Place::Initialize()
{
    std::vector<std::string> stringList;

    stringList.push_back(STR_SOUTH);
    stringList.push_back(STR_EAST);
    stringList.push_back(STR_NORTH);
    stringList.push_back(STR_WEST);

    return stringList;
}
/*****************************************************************************/
Contract::Contract()
    : mContract(PASS)
{

}
/*****************************************************************************/
Contract::Contract(std::uint32_t c)
{
    if (c > GUARD_AGAINST)
    {
        TLogError("Invalid contract value");
        mContract = PASS;
    }
    else
    {
        mContract = static_cast<std::uint8_t>(c);
    }
}
/*****************************************************************************/
Contract::Contract(int c)
{
    if (c > GUARD_AGAINST)
    {
        TLogError("Invalid contract value");
        mContract = PASS;
    }
    else
    {
        mContract = static_cast<std::uint8_t>(c);
    }
}
/*****************************************************************************/
Contract::Contract(std::uint8_t c)
{
    if (c > GUARD_AGAINST)
    {
        TLogError("Invalid contract value");
        mContract = PASS;
    }
    else
    {
        mContract = c;
    }
}
/*****************************************************************************/
Contract::Contract(std::string c)
{
    mContract = PASS;
    for (std::uint32_t i = 0U; i < mStrings.size(); i++)
    {
        if (c == mStrings[i])
        {
            mContract = static_cast<std::uint8_t>(i);
        }
    }
}
/*****************************************************************************/
std::string Contract::ToString() const
{
    return mStrings[mContract];
}
/*****************************************************************************/
std::uint8_t Contract::Value()
{
    return mContract;
}
/*****************************************************************************/
std::vector<std::string> Contract::Initialize()
{
    std::vector<std::string> stringList;

    stringList.push_back(STR_PASS);
    stringList.push_back(STR_TAKE);
    stringList.push_back(STR_GUARD);
    stringList.push_back(STR_GUARD_WITHOUT);
    stringList.push_back(STR_GUARD_AGAINST);

    return stringList;
}

//=============================================================================
// End of file Common.cpp
//=============================================================================
