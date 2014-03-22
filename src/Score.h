/*=============================================================================
 * TarotClub - Score.h
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
#ifndef _SCORE_H
#define _SCORE_H

#include <string>
#include "Common.h"
#include "ByteStreamReader.h"
#include "ByteStreamWriter.h"

class Score
{
public:

    int pointsAttack;   // Points of cards
    int scoreAttack;    // Final score calculated, including all bonuses
    int pointsToDo;
    int difference;
    int multiplier;
    int littleEndianPoints; // positive if attack bonus, otherwise negative
    int handlePoints;
    int slamPoints;

    Team Winner();
    void Reset();
    int GetNumberOfOudlers();
    void SetPointsToDo(int oudlers);
    int GetAttackScore();
    int GetDefenseScore();
    std::string ToString();

    friend ByteStreamWriter &operator<<(ByteStreamWriter &out, Score &info)
    {
        out << (std::uint32_t)info.pointsAttack
            << (std::uint32_t)info.scoreAttack
            << (std::uint32_t)info.pointsToDo
            << (std::uint32_t)info.difference
            << (std::uint32_t)info.multiplier
            << (std::uint32_t)info.littleEndianPoints
            << (std::uint32_t)info.handlePoints
            << (std::uint32_t)info.slamPoints;
        return out;
    }

    friend ByteStreamReader &operator>>(ByteStreamReader &in, Score &info)
    {
        std::uint32_t val;

        in >> val;
        info.pointsAttack = val;
        in >> val;
        info.scoreAttack = val;
        in >> val;
        info.pointsToDo = val;
        in >> val;
        info.difference = val;
        in >> val;
        info.multiplier = val;
        in >> val;
        info.littleEndianPoints = val;
        in >> val;
        info.handlePoints = val;
        in >> val;
        info.slamPoints = val;
        return in;
    }

};

#endif // _SCORE_H

//=============================================================================
// End of file Score.h
//=============================================================================
