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
#include "ServerConfig.h"

/*****************************************************************************/
class Deal
{
public:
    Deal();

    // Helpers
    void NewDeal();
    void StartDeal(Place firstPlayer, const Tarot::Bid &bid);
    void AnalyzeGame(Points &points, std::uint8_t numberOfPlayers);
    void CalculateScore(Points &points);
    std::string GenerateEndDealLog(std::uint8_t numberOfPlayers);
    bool LoadGameDealLog(const std::string &fileName);

    // Getters
    Deck GetTrick(std::uint8_t turn, std::uint8_t numberOfPlayers);
    Place GetWinner(std::uint8_t turn, std::uint8_t numberOfPlayers);
    std::map<int, Place> GetPodium();
    Deck GetDog();
    Deck GetDiscard();

    // Setters
    void SetHandle(const Deck &handle, Team team);
    void SetDiscard(const Deck &discard, Team owner);
    void SetDog(const Deck &dog);
    Place SetTrick(const Deck &trick, std::uint8_t trickCounter);

private:
    Place GetOwner(Place firstPlayer, const Card &c, int turn);

    Deck mDiscard;
    Deck mDog;
    Deck mAttackHandle;
    Deck mDefenseHandle;
    Deck mTricks[24];    // 24 tricks max with 3 players
    Place mWinner[24];
    Place mPreviousWinner;
    Place mFirstPlayer;
    int mTricksWon;
    Deck::Statistics statsAttack;
    Tarot::Bid mBid;

    // Bonus: little oudler (1 of trump) played at the last trick
    bool littleEndianOudler;     // true if the little oudler has been played at the last trick
    Team littleEndianOwner;    // who has won the bonus

    // Bonus: Slam
    bool slamDone;  // true if the slam has been successfully done
    Team slamOwner; // the defense can also perform a slam if everything goes wrong
};


#endif // DEAL_H

//=============================================================================
// End of file Deal.h
//=============================================================================
