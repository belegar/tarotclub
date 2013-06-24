/*=============================================================================
 * TarotClub - Card.h
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

#ifndef _CARD_H
#define _CARD_H

// Game includes
#include "defines.h"
#include <QString>

/*****************************************************************************/
class Card
{

public:
    /**
     * @brief The Color enum, lowest value to highest
     */
    enum Suit
    {
        SPADES = 0,
        HEARTS = 1,
        DIAMONDS = 2,
        CLUBS = 3,
        TRUMPS = 4
    };

    // Constructor/destructor
    Card();

    //  Setters
    void  SetSuit(Suit s);
    void  SetPoints(float p);
    void  SetId(int);
    void  SetOwner(Place);
    void  SetValue(int v);

    // Getters
    Suit    GetSuit();
    float   GetPoints();
    int     GetId();
    Place   GetOwner();
    int     GetValue();
    QString GetName();

    // helpers
    bool IsFool();

private:
    Suit    suit;
    float   points;
    int     id;         // unique identifier
    Place   owner;      // original owner of the card of this deal

    /**
     * @brief The value is the number written on the card
     *
     * Fool: not significant
     * Trumps: [1..21]
     * Other cards: [1..14] with:
     *      King    = 14
     *      Queen   = 13
     *      Knight  = 12
     *      Jack    = 11
     */
    int     value;

};

#endif // _CARD_H

//=============================================================================
// End of file Card.h
//=============================================================================
