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
#ifndef SCORE_H
#define SCORE_H

#include <string>
#include "Common.h"
#include "ByteStreamReader.h"
#include "ByteStreamWriter.h"

class Score
{
public:

    std::int32_t pointsAttack;   // Points of cards
    std::int32_t scoreAttack;    // Final score calculated, including all bonuses
    std::int32_t oudlers;
    std::int32_t littleEndianPoints; // positive if attack bonus, otherwise negative
    std::int32_t handlePoints;
    std::int32_t slamPoints;

    Team Winner() const;
    void Reset();
    std::int32_t GetAttackScore() const;
    std::int32_t GetDefenseScore() const;
    std::int32_t Difference() const;

    friend ByteStreamWriter &operator<<(ByteStreamWriter &out, Score &info)
    {
        out << info.pointsAttack
            << info.scoreAttack
            << info.oudlers
            << info.littleEndianPoints
            << info.handlePoints
            << info.slamPoints;
        return out;
    }

    friend ByteStreamReader &operator>>(ByteStreamReader &in, Score &info)
    {
        in >> info.pointsAttack;
        in >> info.scoreAttack;
        in >> info.oudlers;
        in >> info.littleEndianPoints;
        in >> info.handlePoints;
        in >> info.slamPoints;
        return in;
    }

};

#endif // SCORE_H

//=============================================================================
// End of file Score.h
//=============================================================================
