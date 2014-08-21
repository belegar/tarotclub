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
Team Score::Winner() const
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
void Score::Reset()
{
    pointsAttack        = 0U;
    scoreAttack         = 0U;
    oudlers             = 0U;
    difference          = 0U;
    multiplier          = 0U;
    littleEndianPoints  = 0U;
    handlePoints        = 0U;
    slamPoints          = 0U;
}
/*****************************************************************************/
int Score::GetAttackScore() const
{
    int sign = -1;

    if (Winner() == ATTACK)
    {
        sign = 1;
    }
    return (scoreAttack * sign * 3);
}
/*****************************************************************************/
int Score::GetDefenseScore() const
{
    int sign = 1;

    if (Winner() == ATTACK)
    {
        sign = -1;
    }
    return (scoreAttack * sign);
}

//=============================================================================
// End of file Score.cpp
//=============================================================================
