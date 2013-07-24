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

#include "defines.h"

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
    QString ToString();

    friend QDataStream &operator<<(QDataStream &out, Score &info)
    {
        out << (qint32)info.pointsAttack
            << (qint32)info.scoreAttack
            << (qint32)info.pointsToDo
            << (qint32)info.difference
            << (qint32)info.multiplier
            << (qint32)info.littleEndianPoints
            << (qint32)info.handlePoints
            << (qint32)info.slamPoints;
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, Score &info)
    {
        qint32 val;

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
