/*=============================================================================
 * TarotClub - Deal.h
 *=============================================================================
 * This class stores a complete deal history and calculate final points
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

#ifndef DEAL_H
#define DEAL_H

// Standard includes
#include <map>
#include <list>

// Game includes
#include "Card.h"
#include "Deck.h"
#include "Common.h"
#include "Score.h"
#include "Player.h"
#include "Identity.h"

/*****************************************************************************/
class Deal
{
public:
    Deal();

    // Helpers
    void NewGame();
    void NewDeal();
    void AnalyzeGame(std::uint8_t numberOfPlayers);
    void CalculateScore(const Tarot::Bid &bid, Tarot::Handle attack, Tarot::Handle defense);
    void GenerateEndDealLog(const Tarot::Bid &bid, const std::map<Place, Identity> &players);
    bool LoadGameDealLog(const std::string &fileName);
    bool AddScore(const Tarot::Bid &bid, std::uint8_t numberOfPlayers);

    // Getters
    Deck GetTrick(int turn);
    int  GetTotalPoints(Place p) const;
    Score  &GetScore();
    std::map<int, Place> GetPodium();
    Deck GetDog();

    // Setters
    void SetHandle(const Deck &handle, Team team);
    void SetScore(const Score &score);
    void SetDiscard(const Deck &discard, Team owner);
    Place SetTrick(const Deck &trick, const Tarot::Bid &bid, std::uint8_t trickCounter);

private:
    std::list<std::string> GetSortedTrick(int trick);

    // We store played cards to count points
    Deck mDiscard;
    Deck attackHandleDeck;
    Deck defenseHandleDeck;
    Deck tricks[24];    // 24 tricks max with 3 players
    int tricksWon;
    Deck::Statistics statsAttack;

    // Bonus: little oudler (1 of trump) played at the last trick
    bool littleEndianOudler;     // true if the little oudler has been played at the last trick
    Team littleEndianOwner;    // who has won the bonus

    // Bonus: Slam
    bool slamDone;  // true if the slam has been successfully done
    Team slamOwner; // the defense can also perform a slam if everything goes wrong

    // Bonus: Fool
    bool foolSwap;  // true if the fool has been swaped of teams
    Team foolOwner; // the final owner of the fool

    // Last score
    Score  score;

    // scores of previous deals
    std::uint32_t dealCounter;
    int scores[MAX_ROUNDS][5];   // score of each turn players, 5 players max
};


#endif // DEAL_H

//=============================================================================
// End of file Deal.h
//=============================================================================
