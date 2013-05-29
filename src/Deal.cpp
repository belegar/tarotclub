/*=============================================================================
 * TarotClub - Deal.cpp
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

#include "Deal.h"
#include <stdlib.h>
#include <QFile>
#include <QString>
#include <QTextStream>

/*****************************************************************************/
Deal::Deal()
{
    Initialize();
    reset();
}
/*****************************************************************************/
void Deal::Initialize()
{
    for (int i = 0; i <MAX_ROUNDS; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            deals[i][j] = 0;
        }
    }
    turn = 0;
    carteExcuse = HYPERSPACE;
}
/*****************************************************************************/
/**
 * Est appelé juste avant le premier tour
 */
void Deal::reset()
{
    for (int i = 0; i < 78; i++)
    {
        plis[i] = HYPERSPACE;
    }

    gagne = false;

    chelemDeclare = false;
    chelemRealise = false;
    chelemDefense = false;

    petitAuBout = false;
    petitAttaque = false;

    poigneeDefense = false;
    poigneeAttaque = false;

    typePoigneeD = P_SANS;
    typePoigneeA = P_SANS;

    Deal_inf.pointsAFaire = 0;
    Deal_inf.difference = 0;
    Deal_inf.points_petit_au_bout = 0;
    Deal_inf.multiplicateur = 0;
    Deal_inf.points_poignee = 0;
    Deal_inf.points_chelem = 0;
    Deal_inf.points_defense = 0;
    Deal_inf.attaque = 0.0;
    Deal_inf.defense = 0.0;
    Deal_inf.bouts = 0;
}
/*****************************************************************************/
void Deal::setPli(int i, Place p)
{
    if (i > 77 || i < 0)
    {
        return;
    }
    plis[i] = p;
}
/*****************************************************************************/
void Deal::setPoigneeDefense(Poignee p)
{
    typePoigneeD = p;
}
/*****************************************************************************/
void Deal::setPoigneeAttaque(Poignee p)
{
    typePoigneeA = p;
}
/*****************************************************************************/
Place Deal::getPli(int i)
{
    if (i > 77 || i < 0)
    {
        return HYPERSPACE;
    }
    return plis[i];
}
/*****************************************************************************/
void Deal::setChelemDeclare(bool c)
{
    chelemDeclare = c;
}
/*****************************************************************************/
void Deal::setExcuse(Place p)
{
    carteExcuse = p;
}
/*****************************************************************************/
Place Deal::getExcuse()
{
    return carteExcuse;
}
/*****************************************************************************/
int Deal::getTotalPoints(Place p)
{
    int i;
    int total;

    total = 0;
    for (i = 0; i < turn; i++)
    {
        total += Deals[i][p];
    }
    return(total);
}
/*****************************************************************************/
QList<Place> Deal::getPodium()
{
    QList<Place> placePodium;
    QList<int> pointsPodium;
    int i, j;

    // init lists
    for (i = 0; i < NB_PLAYERS; i++)
    {
        placePodium.append((Place)i);
        pointsPodium.append(Deals[turn - 1][i]);
    }

    // bubble sort
    for (i = 0; i < NB_PLAYERS; i++)
    {
        for (j = 0; j < (NB_PLAYERS - 1); j++)
        {
            if (pointsPodium[j] < pointsPodium[j + 1])
            {
                placePodium.swap(j, j + 1);
                pointsPodium.swap(j, j + 1);
            }
        }
    }

    return placePodium;
}
/*****************************************************************************/
DealInfo &Deal::GetDealInfo()
{
    return Deal_inf;
}
/*****************************************************************************/
void Deal::SetDealInfo(const DealInfo &inf)
{
    Deal_inf = inf;
}
/*****************************************************************************/
/**
 * Calcule les points de la défense et de l'attaque
 */
void Deal::calcul(Deck &mainDeck, Deck &deckChien, GameInfo &info)
{
    int i;
    float n;
    Card *c;
    int flag = 0;

    Deal_inf.bouts = 0;
    Deal_inf.attaque = 0.0;
    Deal_inf.defense = 0.0;
    petitAuBout = false;
    petitAttaque = false;

#ifndef QT_NO_DEBUG
    QString filename = "plis_" + QDateTime::currentDateTime().toString() + ".txt" ;
    QFile out_file(filename);
    out_file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream f(&out_file);
#endif

    for (i = 0; i < 72; i++)
    {
        c = mainDeck.at(i);
        n = c->getPoints();

#ifndef QT_NO_DEBUG
        f << " Couleur : " << c->getColor();
        f << " Type : " << c->getType();
        f << " Valeur : " << c->getValue();
        f << " Owner : " << c->getOwner();
        f << " Pli : " << plis[i];
#endif

        if (c->getType() == EXCUSE)
        {
            if (plis[i] == infos.preneur)
            {
                if (carteExcuse == infos.preneur)
                {
                    Deal_inf.attaque += n;
                    Deal_inf.bouts++;
                }
                else
                {
                    Deal_inf.defense += n;
                    flag = 1;
                }
            }
            else
            {
                if (carteExcuse != infos.preneur)
                {
                    Deal_inf.defense += n;
                }
                else
                {
                    Deal_inf.attaque += n;
                    flag = 1;
                    Deal_inf.bouts++;
                }
            }
        }
        else
        {
            // il doit rendre une carte basse à la place de son excuse
            if (flag == 1 && n == 0.5 && plis[i] == carteExcuse)
            {
                flag = 2;
                if (carteExcuse == infos.preneur)
                {
                    Deal_inf.defense += 0.5;
                }
                else
                {
                    Deal_inf.attaque += 0.5;
                }
            }
            else
            {
                if (plis[i] == infos.preneur)
                {
                    Deal_inf.attaque += n;
                    if (c->getType() == ATOUT)
                    {
                        if (c->getValue() == 21 || c->getValue() == 1)
                        {
                            Deal_inf.bouts++;
                        }
                    }
                }
                else
                {
                    Deal_inf.defense += n;
                }
            }
        }

#ifndef QT_NO_DEBUG
        f << " Attaque : " << Deal_inf.attaque;
        f << " Defense : " << Deal_inf.defense << endl;
#endif

    }


#ifndef QT_NO_DEBUG
    f << "*** Chien ***" << endl;
#endif

    // les points du chien
    n = 0;
    for (i = 0; i < 6; i++)
    {
        c = deckChien.at(i);
        n += c->getPoints();

#ifndef QT_NO_DEBUG
        f << " Couleur : " << c->getColor();
        f << " Type : " << c->getType();
        f << " Valeur : " << c->getValue();
        f << " Owner : " << c->getOwner() << endl;
#endif

        // on compte les possibles bouts dans le chien
        if (infos.contrat == GARDE_SANS)    // seul cas possible avec des bouts dans le Chien
        {
            if (c->getType() == ATOUT)
            {
                if (c->getValue() == 21 || c->getValue() == 1)
                    Deal_inf.bouts++;
            }
            else if (c->getType() == EXCUSE)
                Deal_inf.bouts++;
        }
    }

    if (infos.contrat == GARDE_CONTRE)
    {
        Deal_inf.defense += n;
    }
    else
    {
        Deal_inf.attaque += n;
    }

#ifndef QT_NO_DEBUG
    f << " Attaque : " << Deal_inf.attaque;
    f << " Defense : " << Deal_inf.defense << endl;
    out_file.close();
#endif

    // On teste le Chelem
    if (Deal_inf.attaque == 86.5 && Deal_inf.defense == 4.5)
    {
        // On arrondit, conformément aux règles du Tarot
        Deal_inf.attaque = 87.0;
        Deal_inf.defense = 4.0;
        chelemRealise = true;
        chelemDefense = false;

    }
    else if (Deal_inf.attaque == 4.5 && Deal_inf.defense == 86.5)
    {
        // On arrondit, conformément aux règles du Tarot
        Deal_inf.attaque = 4.0;
        Deal_inf.defense = 87.0;
        chelemRealise = true;
        chelemDefense = true;

    }
    else
    {
        chelemRealise = false;
        chelemDefense = false;
    }

    // recherche du petit au bout
    for (i = 68; i < 72; i++)
    {
        c = mainDeck.at(i);
        if (c->getType() == ATOUT && c->getValue() == 1)
        {
            petitAuBout = true;
            break;
        }
    }

    if (petitAuBout == true)
    {
        // A qui appartient le petit au bout
        // On prend la dernière carte du pli
        if (plis[71] == infos.preneur)
        {
            petitAttaque = true;
        }
    }

    //---------------------------
    // On connait les points des
    // deux camps, on en déduit
    // donc les Deals
    //---------------------------

    if (Deal_inf.bouts == 0)
    {
        Deal_inf.pointsAFaire = 56;
    }
    else if (Deal_inf.bouts == 1)
    {
        Deal_inf.pointsAFaire = 51;
    }
    else if (Deal_inf.bouts == 2)
    {
        Deal_inf.pointsAFaire = 41;
    }
    else
    {
        Deal_inf.pointsAFaire = 36;
    }

    if (infos.contrat == PRISE)
    {
        Deal_inf.multiplicateur = 1;
    }
    else if (infos.contrat == GARDE)
    {
        Deal_inf.multiplicateur = 2;
    }
    else if (infos.contrat == GARDE_SANS)
    {
        Deal_inf.multiplicateur = 4;
    }
    else     // GARDE_CONTRE
    {
        Deal_inf.multiplicateur = 6;
    }

    Deal_inf.difference = (int)(Deal_inf.attaque - Deal_inf.pointsAFaire);

    if (Deal_inf.difference >= 0)
    {
        gagne = true;
    }
    else
    {
        gagne = false;
    }

    Deal_inf.difference = abs(Deal_inf.difference);

    if (petitAuBout == true)
    {
        if (petitAttaque == true)
        {
            Deal_inf.points_petit_au_bout = -10;
        }
        else
        {
            Deal_inf.points_petit_au_bout = 10;
        }
    }
    else
    {
        Deal_inf.points_petit_au_bout = 0;
    }

    // Les points des Poignées déclarées
    if (poigneeDefense == true)
    {
        Deal_inf.points_poignee = 10 + 10 * typePoigneeD;
        if (gagne == true)
        {
            Deal_inf.points_poignee *= -1;   // Les points vont au camp gagnant
        }
    }
    else if (poigneeAttaque == true)
    {
        Deal_inf.points_poignee = (-1) * (10 + 10 * typePoigneeA);
        if (gagne == false)
        {
            Deal_inf.points_poignee *= -1;   // Les points vont au camp gagnant
        }
    }
    else
    {
        Deal_inf.points_poignee = 0;
    }

    // Les points des Chelem déclarés
    if (chelemDeclare == true)
    {
        if (chelemRealise == true)
        {
            if (chelemDefense == true)
            {
                Deal_inf.points_chelem = 200;
            }
            else
            {
                Deal_inf.points_chelem = -400;
            }
        }
        else
        {
            Deal_inf.points_chelem = 200;
        }
    }
    else if (chelemRealise == true)
    {
        if (chelemDefense == true)
        {
            Deal_inf.points_chelem = 200;
        }
        else
        {
            Deal_inf.points_chelem = -200;
        }
    }
    else
    {
        Deal_inf.points_chelem = 0;
    }

    // Le total de points pour chaque défenseur est donc de :
    Deal_inf.points_defense = (25 + abs(Deal_inf.difference) + Deal_inf.points_petit_au_bout) * Deal_inf.multiplicateur +
                               Deal_inf.points_poignee + Deal_inf.points_chelem;

    // Le preneur a rempli son contrat, il prend donc les points à la défense
    if (gagne == true)
    {
        Deal_inf.points_defense *= (-1);
    }

    setPoints(infos);
}
/*****************************************************************************/
void Deal::setPoints(const GameInfos &infos)
{
    int i;

    for (i = 0; i < NB_PLAYERS; i++)
    {
        if (i == infos.preneur)
        {
            Deals[turn][i] = Deal_inf.points_defense * (-3);
        }
        else
        {
            Deals[turn][i] = Deal_inf.points_defense;
        }
    }
    turn++;
}

//=============================================================================
// End of file Deal.h
//=============================================================================
