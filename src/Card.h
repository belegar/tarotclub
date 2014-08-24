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

#ifndef CARD_H
#define CARD_H

// Game includes
#include "Common.h"
#include <string>
#include <cstdint>

/*****************************************************************************/
class Card
{

public:

    typedef std::uint8_t Suit; //!< The Suit enum, lowest value to highest

    static const std::uint8_t SPADES;
    static const std::uint8_t HEARTS;
    static const std::uint8_t DIAMONDS;
    static const std::uint8_t CLUBS;
    static const std::uint8_t TRUMPS;

    // Constructor/destructor
    Card();

    // helpers
    bool IsFool();
    static Suit ToSuit(const std::string &name);
    static std::string ToString(Suit suit);

    //  Setters
    void  SetSuit(Suit s);
    void  SetValue(std::uint8_t v);

    // Getters
    Suit    GetSuit();
    float   GetPoints();
    std::uint8_t     GetValue();
    std::string GetName();

private:
    Suit    mSuit;      //!< Trumps, Hearts, Spades, Diamonds or Clubs

    /**
     * @brief The value is the number written on the card
     *
     * Trumps: [0..21] (Fool is zero)
     * Other cards: [1..14] with:
     *      King    = 14
     *      Queen   = 13
     *      Knight  = 12
     *      Jack    = 11
     */
    std::uint8_t mValue;
};

#endif // CARD_H

//=============================================================================
// End of file Card.h
//=============================================================================
