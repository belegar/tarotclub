/*=============================================================================
 * TarotClub - GameState.h
 *=============================================================================
 * Utility class to store various game information
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
#ifndef GameState_H
#define GameState_H

#include "defines.h"

/*****************************************************************************/
class GameState
{

public:
    enum Sequence
    {
        STOP,
        DEAL,
        BID,
        SHOW_DOG,
        WAIT_DOG,
        DOG,
        BUILD_HANDLE,
        SHOW_HANDLE,
        GAME,
        WAIT_TRICK,
        WAIT_PLAYER
    };

    GameState();

    /**
     * @brief operator ++
     *
     * Overloaded operators to manage game counters
     * Just increment the GameState object each time a players is playing a card
     *
     * @return
     */
    GameState& operator++();
    GameState operator++(int unused);

    void Initialize();

    int GetRemainingTurns();
    int GetNumberOfCards();
    int FirstCard();

    /**
     * @brief Compute the next player to play, counter-clockwise
     * @param j
     * @return
     */
    Place NextPlayer(Place j);

    void Stop();

    /**
     * @brief Start a new deal
     */
    void Start();

    Place       taker;             // who has taken
    Contract    contract;          // taker's contract
    bool        slamAnnounced;     // true if the taker has announced a slam (chelem)
    int         position;          // Current position, [0..numberOfPlayers-1]
    int         numberOfPlayers;   // 3, 4 or 5
    int         trickCounter;      // number of tricks played [0..17] for 4 players
    Place       dealer;            // who has dealt the cards
    Place       currentPlayer;
    Sequence    sequence;

};

#endif // GameState_H

//=============================================================================
// End of file GameState.h
//=============================================================================
