/*=============================================================================
 * TarotClub - GameState.cpp
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

#include "GameState.h"
#include "defines.h"
#include <QMap>

/*****************************************************************************/
GameState::GameState()
{
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime())); // seed init

    Initialize();
}
/*****************************************************************************/
void GameState::Initialize()
{
    position = 0;
    trickCounter = 0;
    numberOfPlayers = 4;
    dealer = static_cast<Place>(qrand() % 4);
}
/*****************************************************************************/
void GameState::Start()
{

}
/*****************************************************************************/
void GameState::Stop()
{
    sequence = STOP;
}
/*****************************************************************************/
/**
 * @brief operator ++
 *
 * Increment the gamecounter, the turn counter is automatically incremented
 * if required, depending of the number of players
 *
 * @return
 */
GameState &GameState::operator++()
{
    position++;
    if (!(position > numberOfPlayers))
    {
        position = 0;
        trickCounter++;
    }
    return *this;
}
/*****************************************************************************/
GameState GameState::operator++(int unused)
{
    Q_UNUSED(unused);
    GameState result = *this;
    ++(*this); // call GameState::operator++()
    return result;
}
/*****************************************************************************/
/**
 * @brief GameState::RemainingTurns
 * @return number of turns remainings
 */
int GameState::GetRemainingTurns()
{
    return (GetNumberOfCards() - trickCounter);
}
/*****************************************************************************/
int GameState::GetNumberOfCards()
{
    if (numberOfPlayers == 3)
    {
        return 24;
    }
    if (numberOfPlayers == 5)
    {
        return 15;
    }
    else
    {
        // 4 players
        return 18;
    }
}
/*****************************************************************************/
int GameState::FirstCard()
{
    return trickCounter*numberOfPlayers;
}
/*****************************************************************************/
Place GameState::NextPlayer(Place j)
{
    Place p;

    if (j == SOUTH)
    {
        p = EAST;
    }
    else if (j == EAST)
    {
        p = NORTH;
    }
    else if (j == NORTH)
    {
        p = WEST;
    }
    else
    {
        p = SOUTH;
    }
    return(p);
}

//=============================================================================
// End of file GameState.cpp
//=============================================================================
