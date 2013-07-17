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
#include "Tools.h"
#include <stdlib.h>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QtXml>

#define DEAL_RESULT_XML_VERSION     "1.0"

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
void Deal::SetTrick(Deck &trick, Game &info)
{
    int turn = info.trickCounter-1;
    if ((turn>=0) && (turn<24))
    {
        tricks[turn] = trick;
        if (info.currentPlayer == info.taker)
        {
            tricks[turn].SetOwner(ATTACK);
        }
        else
        {
            tricks[turn].SetOwner(DEFENSE);
        }
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

    if (info.gameMode == Game::TOURNAMENT)
    {
        dealCounter++;
    }
}
/*****************************************************************************/
void Deal::Calculate(Game &info)
{
    score.Reset();

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

    // 6. We save the points done by the attacker
    score.pointsAttack = statsAttack.points;
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
/*****************************************************************************/
QStringList Deal::GetSortedTrick(int trick)
{
    QStringList list;
    for (int i=0; i<tricks[trick].size(); i++)
    {
        Card *c = tricks[trick].at(i);
        list.insert((int)(c->GetOwner()), c->GetName());
    }

    return list;
}
/*****************************************************************************/
void Deal::SetDog(Deck &dog, Team owner)
{
    dogDeck = dog;
    dogDeck.SetOwner(owner);
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
void Deal::GenerateEndDealLog(Game &info, QMap<Place, Identity> &players)
{
    QDomDocument doc("DealResult");

    doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));
    doc.appendChild(doc.createProcessingInstruction("xml-stylesheet", "type=\"text/xsl\" href=\"tarotclub.xsl\""));
    doc.appendChild(doc.createTextNode("\n"));

    doc.appendChild(doc.createComment(QString("Generated by ") + QString("TarotClub ") + QString(TAROT_VERSION)));
    doc.appendChild(doc.createTextNode("\n"));

    // root node
    QDomElement rootNode = doc.createElement("deal_result");
    rootNode.setAttribute("version", QString(DEAL_RESULT_XML_VERSION));
    doc.appendChild(rootNode);

    // Game information
    QDomElement infoNode = doc.createElement("deal_info");
    rootNode.appendChild(infoNode);

    QDomElement playersNode = doc.createElement("players");
    infoNode.appendChild(playersNode);

    QMapIterator<Place, Identity> i(players);
    while (i.hasNext())
    {
        i.next();

        QDomElement node = doc.createElement("player");
        node.setAttribute("place", Util::ToString(i.key()));
        node.appendChild(doc.createTextNode(i.value().name));

        playersNode.appendChild(node);
    }

    QDomElement takerNode = doc.createElement("taker");
    takerNode.appendChild(doc.createTextNode(Util::ToString(info.taker)));
    infoNode.appendChild(takerNode);

    QDomElement contractNode = doc.createElement("contract");
    contractNode.appendChild(doc.createTextNode(Util::ToString(info.contract)));
    infoNode.appendChild(contractNode);

    QDomElement entameNode = doc.createElement("first_trick_lead");
    entameNode.appendChild(doc.createTextNode(Util::ToString(tricks[0].at(0)->GetOwner())));
    infoNode.appendChild(entameNode);

    // Discard cards
    QDomElement discardNode = doc.createElement("discard");
    discardNode.appendChild(doc.createTextNode(dogDeck.GetCardList()));
    rootNode.appendChild(discardNode);

    // List of tricks and card played
    QDomElement tricksNode = doc.createElement("tricks");
    rootNode.appendChild(tricksNode);
    for (int i=0; i<info.GetNumberOfCards(); i++)
    {
        QDomElement trickNode = doc.createElement("trick");
        trickNode.setAttribute("number", QString::number(i+1));

        QStringList list = GetSortedTrick(i);
        for (int j=0; j<info.numberOfPlayers; j++)
        {
            QDomElement cardNode = doc.createElement("card");
            cardNode.setAttribute("place", Util::ToString((Place)j));
            cardNode.appendChild(doc.createTextNode(list[j]));
            trickNode.appendChild(cardNode);
        }
        tricksNode.appendChild(trickNode);
    }

    // Dump XML document into a file
    QString fileName = Config::LogPath + "/deal_result_" + QDateTime::currentDateTime().toString("ddMMyyyy_hhmmss") + ".xml" ;
    QFile f(fileName);
    if (f.open(QIODevice::WriteOnly) == false)
    {
        return;
    }

    QString out = doc.toString();
    QTextStream fout(&f);
    fout << out;
    f.close();
}
#endif // QT_NO_DEBUG


//=============================================================================
// End of file Deal.h
//=============================================================================
