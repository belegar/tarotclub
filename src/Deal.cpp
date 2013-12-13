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
#include "Log.h"

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
    for (int i = 0; i < MAX_ROUNDS; i++)
    {
        for (int j = 0; j < 5; j++)
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

    for (int i = 0; i < 24; i++)
    {
        tricks[i].clear();
    }

    littleEndianOudler = false;
    slamDone = false;
    slamOwner = NO_TEAM;
    foolSwap = false;
    foolOwner = NO_TEAM;
    tricksWon = 0;
    statsAttack.Reset();
    score.Reset();
}
/*****************************************************************************/
/**
 * @brief SetTrick
 *
 * Store the current trick into memory and calculate the trick winner.
 *
 * This methods does not verify if the trick is consistent with the rules; this
 * is supposed to have been previously verified.
 *
 * @return The place of the winner of this trick
 */
Place Deal::SetTrick(Deck &trick, Game &info)
{
    int turn = info.trickCounter - 1;
    Place winner = NOWHERE;

    // Get the number of tricks we must play, it depends of the number of players
    int numberOfTricks = info.GetNumberOfCards();

    if ((turn >= 0) && (turn < 24))
    {
        tricks[turn] = trick;
        Card *cFool = NULL;
        Card *cLeader = NULL;

        // Each trick is won by the highest trump in it, or the highest card
        // of the suit led if no trumps were played.
        cLeader = trick.HighestTrump();
        if (cLeader == NULL)
        {
            cLeader = trick.HighestSuit();
        }
        if (cLeader == NULL)
        {
            TLogError("cLeader cannot be null!");
        }

        // The trick winner is the card leader owner
        winner = cLeader->GetOwner();

        if (tricks[turn].HasFool())
        {
            cFool = tricks[turn].GetCardByName("0-T");
            if (info.IsDealFinished() == true)
            {
                // Special case of the fool: if played at last turn with a slam realized, it wins the trick
                if ((tricksWon == (numberOfTricks - 1)) &&
                        (cFool->GetOwner() == info.taker))
                {
                    winner = info.taker;
                }
                // Otherwise, the fool is _always_ lost if played at the last trick, even if the
                // fool belongs to the same team than the winner of the trick.
                else
                {
                    foolSwap = true;
                    if (cFool->GetOwner() == info.taker)
                    {
                        foolOwner = DEFENSE;
                    }
                    else
                    {
                        foolOwner = ATTACK;
                    }
                }
            }
            else
            {
                // In all other cases, the fool is kept by the owner. If the trick is won by a
                // different team than the fool owner, they must exchange 1 low card with the fool.
                if (winner != cFool->GetOwner())
                {
                    foolSwap = true;
                    if (winner == info.taker)
                    {
                        foolOwner = DEFENSE;
                    }
                    else
                    {
                        foolOwner = ATTACK;
                    }
                }
                // else, the trick is won by the attacker, it keeps the fool
            }
        }

        if (winner == info.taker)
        {
            tricks[turn].SetOwner(ATTACK);
            tricks[turn].AnalyzeTrumps(statsAttack);
            tricksWon++;
        }
        else
        {
            tricks[turn].SetOwner(DEFENSE);
        }
    }
    else
    {
        TLogError("Index out of scope!");
    }

    return winner;
}
/*****************************************************************************/
void Deal::SetDogOwner(Team team)
{
    dogDeck.SetOwner(team);
}
/*****************************************************************************/
Deck Deal::GetTrick(int turn)
{
    if ((turn < 0) || (turn >= 24))
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
    for (i = 0; i < dealCounter; i++)
    {
        total += scores[i][p];
    }
    return (total);
}
/*****************************************************************************/
QList<Place> Deal::GetPodium()
{
    QList<Place> placePodium;
    QList<int> pointsPodium;
    int i, j;

    // init lists
    for (i = 0; i < 5; i++)
    {
        placePodium.append((Place)i);
        pointsPodium.append(scores[dealCounter - 1][i]);
    }

    // bubble sort
    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < (- 1); j++)
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
void Deal::SetScore(const Score &s)
{
    score = s;
}
/*****************************************************************************/
/**
 * @brief Deal::AddScore
 * @param info
 * @return true if the tournament must continue, false if it is finished
 */
bool Deal::AddScore(const Game &info)
{
    for (int i = 0; i < info.numberOfPlayers; i++)
    {
        if (i == info.taker)
        {
            scores[dealCounter][i] = score.GetAttackScore();
        }
        else
        {
            scores[dealCounter][i] = score.GetDefenseScore();
        }
    }
    dealCounter++;
    if (dealCounter < MAX_ROUNDS)
    {
        return true;
    }
    return false;
}
/*****************************************************************************/
/**
 * @brief Deal::Calculate
 * @param info
 * @return false if the current game has ended
 */
bool Deal::Calculate(Game &info)
{
    AnalyzeGame(info);
    CalculateScore(info);
    if (info.gameMode == Game::TOURNAMENT)
    {
        return AddScore(info);
    }
    return false;
}
/*****************************************************************************/
void Deal::AnalyzeGame(Game &info)
{
    int numberOfTricks = info.GetNumberOfCards();
    int lastTrick = numberOfTricks - 1;

    // 1. Slam detection
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

    // 2. Attacker points, we add the dog if needed
    if (dogDeck.GetOwner() == ATTACK)
    {
        dogDeck.AnalyzeTrumps(statsAttack);
    }

    // 3. Fool owner. If the attacker has lost a fool, it changes some key elements such as:
    //    - The number of oudlers
    //    - The points realized

    if (foolSwap == true)
    {
        if (foolOwner == DEFENSE)
        {
            statsAttack.points -= 4; // defense keeps its points
            statsAttack.oudlers--; // attack looses an oudler! what a pity!
        }
        else
        {
            statsAttack.points += 4; // get back the points
            statsAttack.oudlers++; // hey, it was MY oudler!
        }
    }

    // 4. One of trumps in the last trick bonus detection
    if (slamDone)
    {
        // With a slam, the 1 of Trump bonus is valid if played
        // in the penultimate trick
        lastTrick--;
    }
    if (tricks[lastTrick].HasOneOfTrump())
    {
        littleEndianOudler = true;
        littleEndianOwner = tricks[lastTrick].GetOwner();
    }

    // 5. The number of oudler(s) decides the points to do
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
    for (int i = 0; i < tricks[trick].size(); i++)
    {
        Card *c = tricks[trick].at(i);
        std::string name = c->GetName();
        list.insert((int)(c->GetOwner()), QString(name.data()));
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
    // Open a file where to stream out the XML
    QString fileName = Config::GamePath + "/deal_result_" + QDateTime::currentDateTime().toString("ddMMyyyy_hhmmss") + ".xml" ;
    QFile f(fileName);
    if (f.open(QIODevice::WriteOnly) == true)
    {
        QXmlStreamWriter stream(&f);
        stream.setAutoFormatting(true);
        stream.writeStartDocument();
        stream.writeProcessingInstruction("xml-stylesheet", "type=\"text/xsl\" href=\"tarotclub.xsl\"");

        stream.writeComment(QString("Generated by ") + QString("TarotClub ") + QString(TAROT_VERSION));

        stream.writeStartElement("deal_result");
        stream.writeAttribute("version", QString(DEAL_RESULT_XML_VERSION));

        //========================== Game information ==========================
        stream.writeStartElement("deal_info");

        stream.writeStartElement("players");
        QMapIterator<Place, Identity> i(players);
        while (i.hasNext())
        {
            i.next();

            stream.writeStartElement("player");
            stream.writeAttribute("place", Util::ToString(i.key()));
            stream.writeCharacters(QString(i.value().name.data()));
            stream.writeEndElement(); // player
        }
        stream.writeEndElement(); // players

        stream.writeTextElement("taker", Util::ToString(info.taker));
        stream.writeTextElement("contract", Util::ToString(info.contract));
        stream.writeTextElement("first_trick_lead", Util::ToString(tricks[0].at(0)->GetOwner()));
        stream.writeTextElement("score", score.ToString());

        stream.writeEndElement(); // deal_info

        //========================== Discard cards ==========================
        stream.writeTextElement("discard", QString(dogDeck.GetCardList().data()));

        //========================== Played cards ==========================

        stream.writeStartElement("tricks");

        for (int i = 0; i < info.GetNumberOfCards(); i++)
        {
            stream.writeStartElement("trick");
            stream.writeAttribute("number", QString::number(i + 1));

            QStringList list = GetSortedTrick(i);
            for (int j = 0; j < info.numberOfPlayers; j++)
            {
                stream.writeStartElement("card");
                stream.writeAttribute("place", Util::ToString((Place)j));
                stream.writeCharacters(list[j]);
                stream.writeEndElement(); // card
            }

            stream.writeEndElement(); // trick
        }

        stream.writeEndElement(); // tricks
        stream.writeEndElement(); // deal_result
        stream.writeEndDocument();
        f.close();
    }
}

//=============================================================================
// End of file Deal.h
//=============================================================================
