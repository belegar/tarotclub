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
/**
 * @brief Card class
 *
 * This class represents a Tarot card in memory
 *
 * Trumps: [0..21] (Fool is zero)
 * Other cards: [1..14] with:
 *      King    = 14
 *      Queen   = 13
 *      Knight  = 12
 *      Jack    = 11
 */
class Card
{

public:
    static const std::uint8_t SPADES;
    static const std::uint8_t HEARTS;
    static const std::uint8_t DIAMONDS;
    static const std::uint8_t CLUBS;
    static const std::uint8_t TRUMPS;

    static const std::uint8_t KING;
    static const std::uint8_t QUEEN;
    static const std::uint8_t KNIGHT;
    static const std::uint8_t JACK;

    // Constructors/destructor
    Card();
    Card(const Card &c)
    {
        *this = c;
    }
    explicit Card(std::uint8_t value, std::uint8_t suit);
    explicit Card(const std::string &name);
    explicit Card(const char *name);
    ~Card();

    // Overloaded operators
    Card &operator = (const Card &c);
    bool operator ==  (const Card &c) const;

    // helpers
    bool IsValid() const;
    bool IsFool() const;
    static std::uint8_t SuitFromName(const std::string &name);
    static std::string SuitName(std::uint8_t suit);

    // Getters
    std::uint8_t    GetSuit() const;
    float           GetPoints() const;
    std::uint8_t    GetValue() const;
    std::string     ToString() const;

private:
    std::uint8_t mSuit;      //!< Trumps, Hearts, Spades, Diamonds or Clubs
    std::uint8_t mValue;
};

#endif // CARD_H

//=============================================================================
// End of file Card.h
//=============================================================================
