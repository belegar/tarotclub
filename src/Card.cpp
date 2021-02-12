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
const std::uint8_t INVALID         = 0xFFU;

const std::uint8_t Card::KING   = 14U;
const std::uint8_t Card::QUEEN  = 13U;
const std::uint8_t Card::KNIGHT = 12U;
const std::uint8_t Card::JACK   = 11U;

static const std::string suits = "SHDCT";

/*****************************************************************************/
std::string Card::ToString() const
{
    std::stringstream name;

    // Cast internal value to ingeter to interpret the char as an "integer"
    name << std::setfill('0') << std::setw(2) << (std::uint32_t)mValue << "-" << Card::SuitName(mSuit);
    return name.str();
}
/*****************************************************************************/
/**
 * @brief Card::Card
 *
 * Default constructor builds an invalid card value
 */
Card::Card()
    : mSuit(INVALID)
    , mValue(INVALID)
{
}
/*****************************************************************************/
Card::Card(std::uint8_t value, std::uint8_t suit)
{
    mValue = value;
    mSuit = suit;
}
/*****************************************************************************/
Card::Card(const std::string &name)
    : mSuit(INVALID)
    , mValue(INVALID)
{
    if (name.size() == 4U)
    {
        std::uint32_t value;
        std::stringstream ss(name.substr(0, 2));
        ss >> value;
        mValue = static_cast<std::uint8_t>(value);
        std::string suit;
        suit.push_back(name.at(3));
        mSuit = SuitFromName(suit);
    }
}
/*****************************************************************************/
Card::Card(const char *name)
{
    std::string strname(name);
    *this = Card(strname);
}
/*****************************************************************************/
Card::~Card()
{
    // Nothing to do
}
/*****************************************************************************/
Card &Card::operator = (const Card &c)
{
    mValue = c.mValue;
    mSuit = c.mSuit;
    return *this;
}
/*****************************************************************************/
bool Card::operator ==  (const Card &c) const
{
    bool ret = false;
    if ((c.mSuit == mSuit) && (c.mValue == mValue))
    {
        ret = true;
    }
    return ret;
}
/*****************************************************************************/
bool Card::IsValid() const
{
    bool valid = false;

    // Value must be in the valid range
    if (mSuit == Card::TRUMPS)
    {
        if (mValue <= 21U)
        {
            valid = true;
        }
    }
    else if (mSuit < Card::TRUMPS)
    {
        if ((mValue > 0U) &&
            (mValue <= 14U))
        {
            valid = true;
        }
    }
    else
    {
        valid = false;
    }

    return valid;
}
/*****************************************************************************/
std::uint8_t Card::GetSuit() const
{
    return mSuit;
}
/*****************************************************************************/
float Card::GetPoints() const
{
    float points = 0.5F;

    if (mSuit == TRUMPS)
    {
        // More points for the oudlers
        if ((mValue == 0) ||    // Fool
                (mValue == 1) ||    // 1
                (mValue == 21))      // 21
        {
            points = 4.5F;
        }
    }
    else
    {
        if (mValue > 10U)
        {
            points = 0.5F + (mValue - 10U);
        }
    }
    return points;
}
/*****************************************************************************/
std::uint8_t Card::GetValue() const
{
    return mValue;
}
/*****************************************************************************/
bool Card::IsFool() const
{
    if ((mSuit == Card::TRUMPS) &&
            (mValue == 0U))
    {
        return true;
    }
    else
    {
        return false;
    }
}
/*****************************************************************************/
std::uint8_t Card::SuitFromName(const std::string &name)
{
    std::uint8_t suit = INVALID;
    for (std::uint32_t i = 0U; i < 5U; i++)
    {
        std::string txt;
        txt.push_back(suits.at(i));
        if (name == txt)
        {
            suit = i;
        }
    }
    return suit;
}
/*****************************************************************************/
std::string Card::SuitName(std::uint8_t suit)
{
    std::string name;
    if (suit <= TRUMPS)
    {
        name = suits.at(suit);
    }

    return name;
}

//=============================================================================
// End of file Card.h
//=============================================================================
