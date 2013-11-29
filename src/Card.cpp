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

/*****************************************************************************/
std::string Card::GetName()
{
    std::stringstream name;

    name << value;
    if (suit == SPADES)
    {
        name << "-S"; // spades
    }
    else if (suit == HEARTS)
    {
        name << "-H"; // heart
    }
    else if (suit == CLUBS)
    {
        name << "-C"; // club
    }
    else if (suit == DIAMONDS)
    {
        name << "-D"; // diamond
    }
    else
    {
        name << "-T"; // Trumps
    }
    return name.str();
}
/*****************************************************************************/
Card::Card()
    : suit(Card::CLUBS)
    , points(0.0)
    , id(0)
    , owner(NOWHERE)
{
}
/*****************************************************************************/
void Card::SetSuit(Card::Suit s)
{
    suit = s;
}
/*****************************************************************************/
void Card::SetPoints(float p)
{
    points = p;
}
/*****************************************************************************/
void Card::SetId(int i)
{
    id = i;
}
/*****************************************************************************/
void Card::SetOwner(Place p)
{
    owner = p;
}
/*****************************************************************************/
void Card::SetValue(int v)
{
    value = v;
}
/*****************************************************************************/
Card::Suit Card::GetSuit()
{
    return suit;
}
/*****************************************************************************/
float Card::GetPoints()
{
    return points;
}
/*****************************************************************************/
int Card::GetId()
{
    return id;
}
/*****************************************************************************/
Place Card::GetOwner()
{
    return owner;
}
/*****************************************************************************/
int Card::GetValue()
{
    return value;
}
/*****************************************************************************/
bool Card::IsFool()
{
    if ((suit == Card::TRUMPS) &&
            (value == 0))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//=============================================================================
// End of file Card.h
//=============================================================================
