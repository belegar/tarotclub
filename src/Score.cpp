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

#include <cstdlib>
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
    pointsAttack        = 0;
    oudlers             = 0;
    handlePoints        = 0;
    slamDone            = false;
    littleEndianOwner   = Team::NO_TEAM;
}
/*****************************************************************************/
Team Points::Winner() const
{
    if (pointsAttack >= Tarot::PointsToDo(oudlers))
    {
        return Team(Team::ATTACK);
    }
    else
    {
        return Team(Team::DEFENSE);
    }
}
/*****************************************************************************/
std::int32_t Points::Difference() const
{
    return pointsAttack - Tarot::PointsToDo(oudlers);
}
/*****************************************************************************/
std::int32_t Points::GetSlamPoints(const Tarot::Bid &bid) const
{
    std::int32_t slamPoints = 0;

    if (slamDone)
    {
        if (Winner() == Team::ATTACK)
        {
            if (bid.slam == true)
            {
                slamPoints = 400; // Announced AND realized
            }
            else
            {
                slamPoints = 200; // not announced but realized
            }
        }
        else
        {
            // Defense has won the game and realized a slam!
            slamPoints = 200;
        }
    }
    else
    {
        if (bid.slam == true)
        {
            if (Winner() == Team::ATTACK)
            {
                slamPoints = -200; // Announced but not ealized :(
            }
            else
            {
                slamPoints = 200; // deducted from the winner
            }
        }
        else
        {
            slamPoints = 0;
        }
    }
    return slamPoints;
}
/*****************************************************************************/
std::int32_t Points::GetLittleEndianPoints() const
{
    std::int32_t littleEndianPoints = 0;

    // Little endian bonus:
    // Le camp qui réalise la dernière levée, à condition que cette levée
    // comprenne le Petit, bénéficie d'une prime de 10 points,
    // multipliable selon le contrat, quel que soit le résultat de la donne.
    if (littleEndianOwner == Team::ATTACK)
    {
        // Bonus belong to the attack
        if (Winner() == Team::ATTACK)
        {
            littleEndianPoints = 10; // added to the attack
        }
        else
        {
            littleEndianPoints = -10; // deducted from the defense
        }
    }
    else if (littleEndianOwner == Team::DEFENSE)
    {
        // Bonus belong to the defense
        if (Winner() == Team::ATTACK)
        {
            littleEndianPoints = -10; // deducted to the attack (defense bonus)
        }
        else
        {
            littleEndianPoints = 10; // added to the defense
        }
    }
    else
    {
        // no team, no bonus!
        littleEndianPoints = 0;
    }
    return littleEndianPoints;
}
/*****************************************************************************/
/**
 * @brief Calculate the final score of all players
 *
 * We calculate the points with the following formula:
 *   points = ((25 + pt + pb) * mu) + pg + ch
 *
 * pt = difference between the card points the taker actually won and the minimum number of points he needed
 * pb = the petit au bout bonus is added or subtracted if applicable
 * mu = factor (mu) depending on the bid (1, 2, 4 or 6)
 * pg = the handle bonus (20, 30 or 40)
 * ch = the slam bonus (200 or 400)
 *
 */
std::int32_t Points::GetPoints(const Team team, const Tarot::Bid &bid, uint8_t nbPlayers) const
{
    std::int32_t slamPoints = GetSlamPoints(bid);
    std::int32_t littleEndianPoints = GetLittleEndianPoints();

    // Final scoring
    std::int32_t score = (25 + std::abs(Difference()) + littleEndianPoints) * Tarot::GetMultiplier(bid.contract) + handlePoints + slamPoints;
    std::int32_t sign = 1;
    std::int32_t multiplier = 1;

    if (team == Team::ATTACK)
    {
        if (Winner() == Team::DEFENSE)
        {
            sign = -1;
        }
        multiplier = (nbPlayers - 1);
        if (bid.HasPartner())
        {
            multiplier--;
        }
    }
    else
    {
        if (Winner() == Team::ATTACK)
        {
            sign = -1;
        }
    }

    return (score * sign * multiplier);
}
/*****************************************************************************/



/*****************************************************************************/
Score::Score()
{
    NewGame(TournamentConfig::DEFAULT_NUMBER_OF_TURNS);
}
/*****************************************************************************/
void Score::NewGame(std::uint8_t numberOfTurns)
{
    // Reset tournament information
    for (std::uint32_t i = 0U; i < TournamentConfig::MAX_NUMBER_OF_TURNS; i++)
    {
        for (std::uint32_t j = 0U; j < 5U; j++)
        {
            scores[i][j] = 0;
        }
    }
    mNumberOfTurns = numberOfTurns;
    dealCounter = 0U;
    mHistory.clear();
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
    Score::Entry entry;
    entry.bid = bid;
    entry.points = points;
    entry.nbPlayers = numberOfPlayers;
    mHistory.push_back(entry);

    std::int32_t attackPoints = points.GetPoints(Team(Team::ATTACK), bid, numberOfPlayers);
    std::int32_t defensePoints = points.GetPoints(Team(Team::DEFENSE), bid, numberOfPlayers);

    std::int32_t takerPoints = attackPoints;
    std::int32_t parterPoints = 0;

    // Si en 5 joueurs le preneur a appeté un partenaire
    // alors la répartition est de 2/3 pour le preneur et 1/3 pour le partenaire en attaque
    if ((numberOfPlayers == 5) && bid.HasPartner())
    {
        float deuxTiers = static_cast<float>(attackPoints);
        deuxTiers = deuxTiers * 2 / 3;
        deuxTiers += 0.4; // arrondi sup
        takerPoints = static_cast<int32_t>(deuxTiers);
        parterPoints = attackPoints - takerPoints;
    }

    for (std::uint32_t i = 0U; i < numberOfPlayers; i++)
    {
        if (numberOfPlayers == 5)
        {
            if (Place(i) == bid.taker)
            {
                scores[dealCounter][i] = takerPoints;
            }
            else if ((Place(i) == bid.partner) && bid.HasPartner())
            {
                scores[dealCounter][i] = parterPoints;
            }
            else
            {
                scores[dealCounter][i] = defensePoints;
            }
        }
        else
        {
            if (Place(i) == bid.taker)
            {
                scores[dealCounter][i] = takerPoints;
            }
            else
            {
                scores[dealCounter][i] = defensePoints;
            }
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
int32_t Score::GetTotalPoints(Place p) const
{
    int32_t total;

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
/*****************************************************************************/
Place Score::GetWinner()
{
    return GetPodium().rbegin()->second;
}

//=============================================================================
// End of file Score.cpp
//=============================================================================
