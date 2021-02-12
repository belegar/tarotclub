/*=============================================================================
 * TarotClub - Score.h
 *=============================================================================
 * Helper class to store score for one deal or tournament
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
#ifndef SCORE_H
#define SCORE_H

#include <string>
#include <map>
#include "Common.h"
#include "TournamentConfig.h"

/*****************************************************************************/
struct Points
{
    std::int32_t pointsAttack;   // Points of cards won by the attack
    float cardsPointsAttack; // points des cartes avant ajustements (pour 3 et 5 joueurs)
    std::int32_t oudlers;
    std::int32_t handlePoints;
    bool slamDone;
    Team littleEndianOwner;    // who has won the bonus of the one of trump at last trick

    Points();
    void Clear();
    Team Winner() const;
    std::int32_t Difference() const;
    std::int32_t GetSlamPoints(const Tarot::Bid &bid) const;
    std::int32_t GetLittleEndianPoints() const;
    std::int32_t GetPoints(const Team team, const Tarot::Bid &bid, uint8_t nbPlayers) const;
};
/*****************************************************************************/
class Score
{
public:
    struct Entry
    {
        Points points;
        Tarot::Bid bid;
        uint8_t nbPlayers;
    };

    Score();

    void NewGame(std::uint8_t numberOfTurns);
    void NewDeal();

    std::uint8_t GetNumberOfTurns() { return mNumberOfTurns; }
    std::uint8_t GetCurrentCounter() { return dealCounter; }

    bool AddPoints(const Points &points, const Tarot::Bid &bid, std::uint8_t numberOfPlayers);
    int32_t GetTotalPoints(Place p) const;
    std::map<int, Place> GetPodium();
    Place GetWinner();

    std::vector<Score::Entry> GetHistory() { return mHistory; }

private:
    // scores of previous deals
    std::uint32_t dealCounter;
    int32_t scores[TournamentConfig::MAX_NUMBER_OF_TURNS][5];   // score of each turn players, 5 players max
    std::uint8_t mNumberOfTurns;

    std::vector<Score::Entry> mHistory;
};

#endif // SCORE_H

//=============================================================================
// End of file Score.h
//=============================================================================
