/*=============================================================================
 * TarotClub - Game.h
 *=============================================================================
 * Utility class to store various game state information
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
#ifndef _GAME_H
#define _GAME_H

#include "defines.h"

/*****************************************************************************/
class Game
{

public:
    enum Sequence
    {
        IDLE,
        STOP,
        DEAL,
        BID,
        BUILD_DOG,
        SHOW_DOG,
        BUILD_HANDLE,
        SHOW_HANDLE,
        PLAY_TRICK,
        SYNC_TRICK,
        SYNC_DOG,
        SYNC_READY
    };

    enum Mode
    {
        ONE_DEAL,
        TOURNAMENT
    };

    struct HandleInfo
    {
        Handle type;
        bool declared;
    };

    Game();

    // Helpers
    void Initialize();
    void NewDeal();
    void StartDeal();
    bool Next();
    void Stop();
    bool IsDealFinished();

    // Getters
    int GetRemainingTurns();
    int GetNumberOfCards();
    int GetNumberOfDogCards();

    // Various information of the current deal
    Place       taker;             // who has taken
    Contract    contract;          // taker's contract
    bool        slamAnnounced;     // true if the taker has announced a slam (chelem)
    HandleInfo  attackHandle;
    HandleInfo  defenseHandle;
    int         numberOfPlayers;   // 3, 4 or 5
    Mode        gameMode;

    // Various game states and counters
    int         position;          // Current position, [0..numberOfPlayers-1]
    int         trickCounter;      // number of tricks played [0..17] for 4 players
    Place       dealer;            // who has dealt the cards
    Place       currentPlayer;
    Sequence    sequence;

private:
    /**
     * @brief Compute the next player to play, counter-clockwise
     * @param j
     * @return
     */
    Place NextPlayer(Place j);


};

#endif // _GAME_H

//=============================================================================
// End of file Game.h
//=============================================================================
