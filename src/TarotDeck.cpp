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

            if (j == 10) // Valet
            {
                mCards[n].SetPoints(1.5);
            }
            else if (j == 11)   // Cavalier
            {
                mCards[n].SetPoints(2.5);
            }
            else if (j == 12)   // Dame
            {
                mCards[n].SetPoints(3.5);
            }
            else if (j == 13)   // Roi
            {
                mCards[n].SetPoints(4.5);
            }
            else
            {
                mCards[n].SetPoints(0.5);
            }
        }
    }

    // The 22 trumps, including the fool
    for (i = 56; i < 78; i++)
    {
        mCards[i].SetValue(i - 56); // [0..21]
        mCards[i].SetSuit(Card::TRUMPS);

        // More points for the oudlers
        if ((i == 56) ||    // Fool
                (i == 57) ||    // 1
                (i == 77))      // 21
        {
            mCards[i].SetPoints(4.5);
        }
        else
        {
            mCards[i].SetPoints(0.5);
        }
    }
}
/*****************************************************************************/
Card *TarotDeck::GetCard(int index)
{
    if ((index < 0) || (index > 77))
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
    for (int i=0; i<78; i++)
    {
        if (name == mCards[i].GetName())
        {
            c = &mCards[i];
        }
    }
    return c;
}

//=============================================================================
// End of file TarotDeck.cpp
//=============================================================================
