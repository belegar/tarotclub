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
    NewDeal();
}
/*****************************************************************************/
void Deal::Initialize()
{
    for (int i = 0; i<MAX_ROUNDS; i++)
    {
        for (int j = 0; j<5; j++)
        {
            scores[i][j] = 0;
        }
    }
    dealCounter = 0;
}
/*****************************************************************************/
void Deal::NewDeal()
{
    dogDeck.clear();
    attackHandleDeck.clear();
    defenseHandleDeck.clear();

    for (int i=0; i<24; i++)
    {
        tricks[i].clear();
    }

    littleEndianOudler = false;
    slamDone = false;
}
/*****************************************************************************/
void Deal::SetTrick(Deck &trick, int turn)
{
    turn--;
    if ((turn>=0) && (turn<24))
    {
        tricks[turn] = trick;
    }
}
/*****************************************************************************/
void Deal::SetDogOwner(Team team)
{
    dogDeck.SetOwner(team);
}
/*****************************************************************************/
Deck Deal::GetTrick(int turn)
{
    if ((turn<0) || (turn>=24))
    {
        turn = 0;
    }
    return tricks[turn];
}
/*****************************************************************************/
void Deal::SetHandle(Deck &handle, Team team)
{
    if (team == ATTACK)
    {
        attackHandleDeck = handle;
    }
    else
    {
        defenseHandleDeck = handle;
    }
}
/*****************************************************************************/
int Deal::GetTotalPoints(Place p)
{
    int i;
    int total;

    total = 0;
    for (i = 0; i<dealCounter; i++)
    {
        total += scores[i][p];
    }
    return(total);
}
/*****************************************************************************/
QList<Place> Deal::GetPodium()
{
    QList<Place> placePodium;


    /* FIXME uncomment and recode the algorithm

    QList<int> pointsPodium;
    int i, j;

    // init lists
    for (i = 0; i<5; i++)
    {
        placePodium.append((Place)i);
        pointsPodium.append(scores[dealCounter - 1][i]);
    }

    // bubble sort
    for (i = 0; i<5; i++)
    {
        for (j = 0; j<( - 1); j++)
        {
            if (pointsPodium[j] < pointsPodium[j + 1])
            {
                placePodium.swap(j, j + 1);
                pointsPodium.swap(j, j + 1);
            }
        }
    }
*/
    return placePodium;
}
/*****************************************************************************/
Deck &Deal::GetDog()
{
    return dogDeck;
}
/*****************************************************************************/
Score &Deal::GetScore()
{
    return score;
}
/*****************************************************************************/
void Deal::SetScore(const Score &s, const Game &info)
{
    score = s;

    if (info.gameMode == Game::LOCAL_TOURNAMENT)
    {
        for (int i = 0; i<info.numberOfPlayers; i++)
        {
            if (i == info.taker)
            {
                scores[dealCounter][i] = s.scoreAttack;
            }
            else
            {
                scores[dealCounter][i] = s.scoreAttack / (-3);
            }
        }
        dealCounter++;
    }
}
/*****************************************************************************/
#ifndef QT_NO_DEBUG
/**
 * @brief Generate a file with all played cards of the deal
 *
 * This file also contains useful information such as ower of cards, points ...
 * The log contains 2 parts, the main deck and the dog, each parts has 4 lines
 * line 1: name of cards
 * line 2: points of each card
 * line 3: the original owner of the card
 * line 4: the final owner of the card
 */
void Deal::GenerateEndDealLog()
{
    /*

    QString filename = "round_cards_" + QDateTime::currentDateTime().toString() + ".csv" ;

    ofstream f(filename);

    QString line1 = "Card;" + mainDeck.GetCardList();
    QString line2 = "Points";
    QString line3 = "Base owner";
    QString line4 = "Final owner";

    // Card type
    for (int j = 0; j < mainDeck.size(); j++)
    {
        QString n;
        Card *c = mainDeck.at(j);
        line2 += ";" + n.setNum(c->getPoints());
        line3 += ";" + n.setNum((int)c->getOwner());
        line4 += ";" + n.setNum(score.getPli(j));
    }

    f << "Main deck" << endl;
    f << line1.toStdString() << "\n" << line2.toStdString() << "\n" << line3.toStdString() << "\n" << line4.toStdString() << "\n\n";

    line1 = line2 = line3 = line4 = "";

    // Card type
    line1 = dogDeck.GetCardList();
    for (int j = 0; j < dogDeck.size(); j++)
    {
        QString n;
        Card *c = dogDeck.at(j);
        line2 += n.setNum(c->GetPoints()) + ";";
        line3 += n.setNum((int)c->GetOwner()) + ";";
        line4 += n.setNum(score.getPli(j)) + ";";
    }
    f << "Dog deck" << endl;
    f << line1.toStdString() << "\n" << line2.toStdString() << "\n" << line3.toStdString() << "\n" << line4.toStdString() << "\n\n";

    f << "Game infos" << endl;
    f << "Taker;" << infos.preneur << endl << "Bid;" << infos.contrat << endl;

    f.close();
        */
}
#endif // QT_NO_DEBUG
/*****************************************************************************/
void Deal::Calculate(Game &info)
{
    score.Reset();

#ifndef QT_NO_DEBUG
    GenerateEndDealLog();
#endif

    AnalyzeGame(info);
    CalculateScore(info);
}
/*****************************************************************************/
/**
 * @brief Deal::AnalyzeGame
 *
 * 1. Attacker points
 * 2. Slam detection
 * 3. 1 of Trump at last trick detection (attack or defense?)
 * 4. Fool owner (lost if played during last trick, except in case of Chelem)
 * 5. Oudler counter decides points to do
 *
 * @param info
 */
void Deal::AnalyzeGame(Game &info)
{
    int i;
    Deck::Statistics statsAttack;
    int tricksWon;

    // Get the number of tricks played depending of the number of players
    int numberOfTricks = info.GetNumberOfCards();

    // 1. Attacker points
    statsAttack.Reset();
    tricksWon = 0;
    for (i = 0; i<numberOfTricks; i++)
    {
        if (tricks[i].GetOwner() == ATTACK)
        {
            tricks[i].AnalyzeTrumps(statsAttack);
            tricksWon++;
        }
    }
    // We add the dog if needed
    if (dogDeck.GetOwner() == ATTACK)
    {
        dogDeck.AnalyzeTrumps(statsAttack);
    }

    // 2. Slam detection
    if (tricksWon == numberOfTricks)
    {
        slamDone = true;
        slamOwner = ATTACK;
    }
    if (tricksWon == 0)
    {
        slamDone = true;
        slamOwner = DEFENSE;
    }

    // 3. 1 of Trump at last trick detection (attack or defense?)
    i = numberOfTricks-1; // last trick
    if (slamDone)
    {
        // With a slam, the 1 of Trump bonus is valid if played
        // in the penultimate trick
        i--;
    }
    if (tricks[i].HasOneOfTrump())
    {
        littleEndianOudler = true;
        littleEndianOwner = tricks[i].GetOwner();
    }

    // 4. Fool owner (lost if played during last trick, except in case of Chelem)

    // FIXME Complete

    // 5. Oudler counter decides points to do
    score.SetPointsToDo(statsAttack.oudlers);
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
void Deal::CalculateScore(Game &info)
{
    if (info.contract == TAKE)
    {
        score.multiplier = 1;
    }
    else if (info.contract == GUARD)
    {
        score.multiplier = 2;
    }
    else if (info.contract == GUARD_WITHOUT)
    {
        score.multiplier = 4;
    }
    else     // GUARD_AGAINST
    {
        score.multiplier = 6;
    }

    // Handle bonus: Ces primes gardent la même valeur quel que soit le contrat.
    // La prime est acquise au camp vainqueur de la donne.
    if (info.attackHandle.declared)
    {
        score.handlePoints += GetHandlePoints(info.attackHandle.type);
    }
    if (info.defenseHandle.declared)
    {
        score.handlePoints += GetHandlePoints(info.defenseHandle.type);
    }

    // Little endian bonus:
    // Le camp qui réalise la dernière levée, à condition que cette levée
    // comprenne le Petit, bénéficie d'une prime de 10 points,
    // multipliable selon le contrat, quel que soit le résultat de la donne.
    if (littleEndianOudler == true)
    {
        if (littleEndianOwner == score.Winner())
        {
            score.littleEndianPoints = 10;
        }
        else
        {
            score.littleEndianPoints = -10;
        }
    }

    // Slam bonus
    if (slamDone)
    {
        if (info.slamAnnounced == true)
        {
            score.slamPoints = 400;
        }
        else
        {
            score.slamPoints = 200;
        }
    }
    else
    {
        // announced but not realized
        if (info.slamAnnounced == true)
        {
            score.slamPoints = -200;
        }
    }

    score.difference = score.pointsAttack - score.pointsToDo;
    // Final scoring
    score.scoreAttack = (25 + abs(score.difference) + score.littleEndianPoints) * score.multiplier + score.handlePoints + score.slamPoints;
}
/*****************************************************************************/
int Deal::GetHandlePoints(Handle h)
{
    if (h == SIMPLE_HANDLE)
    {
        return 20;
    }
    else if (h == DOUBLE_HANDLE)
    {
        return 30;
    }
    else
    {
        return 40;
    }
}

//=============================================================================
// End of file Deal.h
//=============================================================================
