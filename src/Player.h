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

private:
    Identity    identity;   // player's identification
    Deck        myDeck;     // players cards
    Place       place;   // place assignée par le serveur autour de la table

public:
    Player();

    void SetIdentity(Identity &ident);
    void SetPlace(Place p);

    Identity &GetIdentity();
    Place GetPlace();
    Deck &GetDeck();

    bool CanPlayCard(Card *, Deck &trick, Game &info);
};

#endif // _PLAYER_H

//=============================================================================
// End of file Player.h
//=============================================================================
