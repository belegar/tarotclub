/*=============================================================================
 * TarotClub - Player.h
 *=============================================================================
 * Basic modelization of a tarot player
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

#ifndef _PLAYER_H
#define _PLAYER_H

#include "defines.h"
#include "Deck.h"
#include "Identity.h"
#include "Game.h"

/*****************************************************************************/
class Player
{
public:
    Player();

    // Helpers
    bool CanPlayCard(Card *, Deck &trick);
    bool IsFree();

    // Getters
    Identity &GetIdentity();
    Place GetPlace();
    Deck &GetDeck();
    std::uint32_t GetUuid() const;

    // Setters
    void SetIdentity(const Identity &ident);
    void SetPlace(Place p);
    void SetUuid(std::uint32_t value);

private:
    Identity    mIdentity;  //!< player's identification
    Deck        mDeck;      //!< players cards
    Place       mPlace;     //!< assigned place by the server around the table
    std::uint32_t mUuid;    //!< User's unique identifier
};

#endif // _PLAYER_H

//=============================================================================
// End of file Player.h
//=============================================================================
