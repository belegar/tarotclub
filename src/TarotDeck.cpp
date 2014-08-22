/*=============================================================================
 * TarotClub - TarotDeck.cpp
 *=============================================================================
 * Static class in memory to store cards of the game.
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

#include "TarotDeck.h"

Card TarotDeck::mCards[78];

/*****************************************************************************/
TarotDeck::TarotDeck()
{

}
/*****************************************************************************/
void TarotDeck::Initialize()
{
    int i, j, n;

    // The 4 suits
    for (i = 0; i < 4; i++)
    {
        // From ace to the king, 14 cards
        for (j = 0; j < 14; j++)
        {
            n = i * 14 + j;
            mCards[n].SetSuit(i);
            mCards[n].SetValue(j + 1);  // [1..14]
        }
    }

    // The 22 trumps, including the fool
    for (i = 56; i < 78; i++)
    {
        mCards[i].SetValue(i - 56); // [0..21]
        mCards[i].SetSuit(Card::TRUMPS);
    }
}
/*****************************************************************************/
Card *TarotDeck::GetCard(std::uint8_t index)
{
    if (index > 77U)
    {
        return NULL;
    }
    else
    {
        return (&mCards[index]);
    }
}
/*****************************************************************************/
Card *TarotDeck::GetCard(const std::string &name)
{
    Card *c = NULL;
    for (uint8_t i = 0U; i < 78U; i++)
    {
        if (name == mCards[i].GetName())
        {
            c = &mCards[i];
        }
    }
    return c;
}
/*****************************************************************************/
std::uint8_t TarotDeck::GetIndex(const std::string &name)
{
    uint8_t index = 0U;
    for (uint8_t i = 0U; i < 78U; i++)
    {
        if (name == mCards[i].GetName())
        {
            index = i;
        }
    }
    return index;
}

//=============================================================================
// End of file TarotDeck.cpp
//=============================================================================
