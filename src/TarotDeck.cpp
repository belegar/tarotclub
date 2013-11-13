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

Card TarotDeck::cards[78];

/*****************************************************************************/
TarotDeck::TarotDeck()
{

}
/*****************************************************************************/
void TarotDeck::Initialize()
{
    int i, j, n;
    Card::Suit suit;
    int id;

    // The 4 suits
    for (i = 0; i < 4; i++)
    {
        if (i == 0)
        {
            suit = Card::SPADES;
            id = 0;
        }
        else if (i == 1)
        {
            suit = Card::HEARTS;
            id = 14;
        }
        else if (i == 2)
        {
            suit = Card::CLUBS;
            id = 28;
        }
        else
        {
            suit = Card::DIAMONDS;
            id = 42;
        }

        // From ace to the king, 14 cards
        for (j = 0; j < 14; j++)
        {
            n = i * 14 + j;
            cards[n].SetSuit(suit);
            cards[n].SetValue(j + 1);  // [1..14]
            cards[n].SetId(id + j);

            if (j == 10) // Valet
            {
                cards[n].SetPoints(1.5);
            }
            else if (j == 11)   // Cavalier
            {
                cards[n].SetPoints(2.5);
            }
            else if (j == 12)   // Dame
            {
                cards[n].SetPoints(3.5);
            }
            else if (j == 13)   // Roi
            {
                cards[n].SetPoints(4.5);
            }
            else
            {
                cards[n].SetPoints(0.5);
            }
        }
    }

    // The 22 trumps, including the fool
    for (i = 56; i < 78; i++)
    {
        cards[i].SetValue(i - 56); // [0..21]
        cards[i].SetSuit(Card::TRUMPS);
        cards[i].SetId(i);

        // More points for the oudlers
        if ((i == 56) ||    // Fool
            (i == 57) ||    // 1
            (i == 77))      // 21
        {
            cards[i].SetPoints(4.5);
        }
        else
        {
            cards[i].SetPoints(0.5);
        }
    }
}
/*****************************************************************************/
Card *TarotDeck::GetCard(int id)
{
    if ((id < 0) || (id > 77))
    {
        return NULL;
    }
    else
    {
        return(&cards[id]);
    }
}
/*****************************************************************************/
Card *TarotDeck::GetCard(const QString &name)
{
    Card *c = NULL;
    for (int i=0; i<78; i++)
    {
        if (name == cards[i].GetName())
        {
            c = &cards[i];
        }
    }
    return c;
}

//=============================================================================
// End of file TarotDeck.cpp
//=============================================================================
