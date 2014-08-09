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

#include "Common.h"
#include "Deck.h"

/*****************************************************************************/
class Player : public Deck
{
public:
    Player();

    // Helpers
    bool CanPlayCard(Card *, Deck &trick);
    bool IsFree();
    bool TestHandle(const Deck &handle);
    bool TestDiscard(const Deck &discard, const Deck &dog, std::uint8_t numberOfPlayers);

    /**
     * @brief RemoveDuplicates
     *
     * Remove all the cards in the player's deck that exist in the "cards" deck passed in argument
     *
     * @param cards
     */
    void RemoveDuplicates(const Deck &cards);

    // Getters
    std::uint32_t GetUuid() const;
    bool HasAck()
    {
        return mAck;
    }

    // Setters
    void SetUuid(std::uint32_t value);
    void SetAck(bool ack = true)
    {
        mAck = ack;
    }

private:
    // player's cards in hand
    struct PlayerStats
    {
        bool hasSuit;           // true if the player has the requested color
        bool hasTrump;          // true if the player has some trumps
        int  highestTrumpValue; // value of the maximum trump in hand
        bool previousTrump;     // true if there is previous trump played
        int  maxPreviousTrump;  // maximum value of the previous trump played

        PlayerStats()
        {
            hasSuit = false;
            hasTrump = false;
            highestTrumpValue = 0;
            previousTrump = false;
            maxPreviousTrump = 0;
        }
    };

    std::uint32_t mUuid;    //!< User's unique identifier
    bool mAck;


    bool TestPlayTrump(Card *cVerif, const PlayerStats &stats);
};

#endif // _PLAYER_H

//=============================================================================
// End of file Player.h
//=============================================================================
