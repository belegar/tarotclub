/*=============================================================================
 * TarotClub - Card.cpp
 *=============================================================================
 * Abstract class representing Tarot cards caracteristics
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

#include "Card.h"
#include <sstream>
#include <map>
#include <iomanip>

const std::uint8_t Card::SPADES    = 0U;
const std::uint8_t Card::HEARTS    = 1U;
const std::uint8_t Card::DIAMONDS  = 2U;
const std::uint8_t Card::CLUBS     = 3U;
const std::uint8_t Card::TRUMPS    = 4U;

/*****************************************************************************/
std::string Card::GetName()
{
    std::stringstream name;

    name << std::setfill('0') << std::setw(2) << mValue << "-" << Card::ToString(mSuit);
    return name.str();
}
/*****************************************************************************/
Card::Card()
    : mSuit(Card::CLUBS)
    , mPoints(0.0)
    , mId(0)
    , mValue(0)
{
}
/*****************************************************************************/
void Card::SetSuit(Card::Suit s)
{
    mSuit = s;
}
/*****************************************************************************/
void Card::SetPoints(float p)
{
    mPoints = p;
}
/*****************************************************************************/
void Card::SetId(int i)
{
    mId = i;
}
/*****************************************************************************/
void Card::SetOwner(Place p)
{
    mOwner = p;
}
/*****************************************************************************/
void Card::SetValue(int v)
{
    mValue = v;
}
/*****************************************************************************/
Card::Suit Card::GetSuit()
{
    return mSuit;
}
/*****************************************************************************/
float Card::GetPoints()
{
    return mPoints;
}
/*****************************************************************************/
int Card::GetId()
{
    return mId;
}
/*****************************************************************************/
Place Card::GetOwner()
{
    return mOwner;
}
/*****************************************************************************/
int Card::GetValue()
{
    return mValue;
}
/*****************************************************************************/
bool Card::IsFool()
{
    if ((mSuit == Card::TRUMPS) &&
            (mValue == 0))
    {
        return true;
    }
    else
    {
        return false;
    }
}
/*****************************************************************************/
Card::Suit Card::ToSuit(const std::string &name)
{
    std::map<std::string, Suit> m;
    m["T"] = TRUMPS;
    m["H"] = HEARTS;
    m["D"] = DIAMONDS;
    m["C"] = CLUBS;
    m["S"] = SPADES;

    return m[name];
}
/*****************************************************************************/
std::string Card::ToString(Card::Suit suit)
{
    std::map<Suit, std::string> m;
    m[TRUMPS]   = "T";
    m[HEARTS]   = "H";
    m[DIAMONDS] = "D";
    m[CLUBS]    = "C";
    m[SPADES]   = "S";

    return m[suit];
}

//=============================================================================
// End of file Card.h
//=============================================================================
