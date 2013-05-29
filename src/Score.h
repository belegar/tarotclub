/*=============================================================================
 * TarotClub - ScoreInfo.h
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

class Score
{
public:

    float attaque;
    float defense;
    int   bouts;
    int   pointsAFaire;
    int   difference;
    int   points_petit_au_bout;
    int   multiplicateur;
    int   points_poignee;
    int   points_chelem;
    int   points_defense;

    friend QDataStream &operator<<(QDataStream &out, ScoreInfo &info)
    {
        out << (qint32)info.attaque
            << (qint32)info.defense
            << (qint32)info.bouts
            << (qint32)info.pointsAFaire
            << (qint32)info.difference
            << (qint32)info.points_petit_au_bout
            << (qint32)info.multiplicateur
            << (qint32)info.points_poignee
            << (qint32)info.points_chelem
            << (qint32)info.points_defense;
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, ScoreInfo &info)
    {
        qint32 val32;
        float valF;

        in >> valF;
        info.attaque = valF;
        in >> valF;
        info.defense = valF;
        in >> val32;
        info.bouts = val32;
        in >> val32;
        info.pointsAFaire = val32;
        in >> val32;
        info.difference = val32;
        in >> val32;
        info.points_petit_au_bout = val32;
        in >> val32;
        info.multiplicateur = val32;
        in >> val32;
        info.points_poignee = val32;
        in >> val32;
        info.points_chelem = val32;
        in >> val32;
        info.points_defense = val32;
        return in;
    }

};

#endif // _SCORE_H

//=============================================================================
// End of file ScoreInfo.h
//=============================================================================
