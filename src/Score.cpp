/*=============================================================================
 * TarotClub - Score.cpp
 *=============================================================================
 * Helper class that stores various scoring information
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

#include "Score.h"
#include "Common.h"

/*****************************************************************************/
Points::Points()
{
    Clear();
}
/*****************************************************************************/
void Points::Clear()
{
    // Points for one deal
    pointsAttack        = 0U;
    scoreAttack         = 0U;
    oudlers             = 0U;
    littleEndianPoints  = 0U;
    handlePoints        = 0U;
    slamPoints          = 0U;
}
/*****************************************************************************/
Team Points::Winner() const
{
    if (pointsAttack >= Tarot::PointsToDo(oudlers))
    {
        return ATTACK;
    }
    else
    {
        return DEFENSE;
    }
}
/*****************************************************************************/
std::int32_t Points::GetAttackScore() const
{
    std::int32_t sign = -1;

    if (Winner() == ATTACK)
    {
        sign = 1;
    }
    return (scoreAttack * sign * 3);
}
/*****************************************************************************/
std::int32_t Points::GetDefenseScore() const
{
    std::int32_t sign = 1;

    if (Winner() == ATTACK)
    {
        sign = -1;
    }
    return (scoreAttack * sign);
}
/*****************************************************************************/
std::int32_t Points::Difference() const
{
    return pointsAttack - Tarot::PointsToDo(oudlers);
}
/*****************************************************************************/


/*****************************************************************************/
Score::Score()
{
    NewGame(ServerConfig::DEFAULT_NUMBER_OF_TURNS);
}
/*****************************************************************************/
void Score::NewGame(std::uint8_t numberOfTurns)
{
    // Reset tournament information
    for (std::uint32_t i = 0U; i < ServerConfig::MAX_NUMBER_OF_TURNS; i++)
    {
        for (std::uint32_t j = 0U; j < 5U; j++)
        {
            scores[i][j] = 0;
        }
    }
    mNumberOfTurns = numberOfTurns;
    dealCounter = 0U;
}
/*****************************************************************************/
void Score::NewDeal()
{
    // Manage rollover
    if (dealCounter >= mNumberOfTurns)
    {
        dealCounter = 0U;
    }
}
/*****************************************************************************/
/**
 * @brief Add the current score to the tournament
 * @param info
 * @return true if the tournament must continue, false if it is finished
 */
bool Score::AddPoints(const Points &points, const Tarot::Bid &bid, std::uint8_t numberOfPlayers)
{
    for (std::uint32_t i = 0U; i < numberOfPlayers; i++)
    {
        if (Place(i) == bid.taker)
        {
            scores[dealCounter][i] = points.GetAttackScore();
        }
        else
        {
            scores[dealCounter][i] = points.GetDefenseScore();
        }
    }
    dealCounter++;
    if (dealCounter < mNumberOfTurns)
    {
        return true;
    }
    return false;
}
/*****************************************************************************/
int Score::GetTotalPoints(Place p) const
{
    int total;

    total = 0;
    for (std::uint32_t i = 0U; i < dealCounter; i++)
    {
        total += scores[i][p.Value()];
    }
    return (total);
}
/*****************************************************************************/
/**
 * @brief Score::GetPodium
 *
 * Gets the podium of the tournament. The first player has the highest number of points.
 *
 * @return The sorted list of players, by points
 */
std::map<int, Place> Score::GetPodium()
{
    std::map<int, Place> podium;

    if (dealCounter > 0U)
    {
        // init podium
        for (std::uint32_t i = 0U; i < 5U; i++)
        {
            int points = scores[dealCounter - 1U][i];
            podium[points] = (Place)i;
        }
    }

    // Since the map naturally sorts the list in the key order (points)
    // there is no need to sort anything
    return podium;
}

//=============================================================================
// End of file Score.cpp
//=============================================================================
