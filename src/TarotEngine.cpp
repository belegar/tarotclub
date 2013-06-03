/*=============================================================================
 * TarotClub - TarotEngine.cpp
 *=============================================================================
 * Moteur de jeu principal + serveur de jeu réseau
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

#include <QtNetwork>
#include <QCoreApplication>
#include "TarotEngine.h"
#include "DealFile.h"
#include "Identity.h"

#ifndef QT_NO_DEBUG
#include <iostream>
#include <fstream>
using namespace std;
#endif // QT_NO_DEBUG

/*****************************************************************************/
TarotEngine::TarotEngine()
{
    dealNumber = 0;
    dealType = RANDOM_DEAL;
    gameMode = LOCAL_ONEDEAL;
}
/*****************************************************************************/
TarotEngine::~TarotEngine()
{

}
/*****************************************************************************/
void TarotEngine::SetGameMode(GameMode mode)
{
    gameMode = mode;
}
/*****************************************************************************/
void TarotEngine::SetDealNumber(int deal)
{
    dealNumber = deal;
}
/*****************************************************************************/
void TarotEngine::SetDealType(DealType type)
{
    dealType = type;
}
/*****************************************************************************/
void TarotEngine::SetDealFile(QString file)
{
    dealFile = file;
}
/*****************************************************************************/
void TarotEngine::StopGame()
{
    gameState.Stop();
}
/*****************************************************************************/
Player &TarotEngine::GetPlayer(Place p)
{
    return players[p];
}
/*****************************************************************************/
Score &TarotEngine::GetScore()
{
    return deal.GetScore();
}
/*****************************************************************************/
int TarotEngine::GetDealNumber()
{
    return dealNumber;
}
/*****************************************************************************/
bool TarotEngine::IsCardValid(Card *c, Place p)
{
    bool ret = false;

    if (gameState.sequence == Game::BUILD_DOG)
    {
        if ((c->GetSuit() == Card::TRUMPS) ||
            ((c->GetSuit() != Card::TRUMPS) && (c->GetValue() == 14)))
        {
            ret = false;
        }
        else
        {
            ret = true;
        }
    }
    else if (gameState.sequence == Game::PLAY_TRICK)
    {
        ret =  players[p].CanPlayCard(c, &currentTrick, gameState);
    }

    return (ret);
}
/*****************************************************************************/
bool TarotEngine::HasCard(Card *c, Place p)
{
    return players[p].GetDeck().HasCard(c);
}
/*****************************************************************************/
Place TarotEngine::CalculateTrickWinner()
{
    Card *cLeader;
    int maxValue = 0;
    bool trumpDetected = false;

    cLeader = currentTrick.at(0);   // First card played this trick

    for (int i = 0; i<currentTrick.size(); i++)
    {
        Card *c = currentTrick.at(i);
        int value = c->GetValue();

        if (c->GetSuit() == Card::TRUMPS)
        {
            trumpDetected = true;
            if (value > maxValue)
            {
                maxValue = value;
                cLeader = c;
            }
        }
        else if (trumpDetected == false)
        {
            if (value > maxValue)
            {
                maxValue = value;
                cLeader = c;
            }
        }
    }

    return cLeader->GetOwner();
}
/*****************************************************************************/
void TarotEngine::NewDeal()
{
    // 1. Give cards to all players
    CreateDeal();

    // 2. Initialize internal states
    deal.NewDeal();
    gameState.NewDeal();

    // 3. Start the bid sequence
    emit sigSelectPlayer(gameState.currentPlayer);
    emit sigAskBid(gameState.contract);
}

/*****************************************************************************/
void TarotEngine::GameSateMachine()
{
    if (gameState.Next() == true)
    {
        // The current trick winner will begin the next trick
        gameState.currentPlayer = CalculateTrickWinner();
        deal.SetTrick(currentTrick, gameState.trickCounter);

        if (gameState.IsDealFinished() == true)
        {
            deal.Calculate(gameState);
            dealCounter++;

            // Manage tournaments
            if ((dealCounter<MAX_ROUNDS) && (gameMode == LOCAL_TOURNAMENT))
            {
                 gameState.sequence = Game::WAIT_PLAYER;
            }
            else
            {
                 gameState.sequence = Game::STOP;
            }

            cptVuDonne = 0;
            emit sigEndOfDeal();
        }
        else
        {
            cptVuPli = 0;
            gameState.sequence = Game::WAIT_TRICK;
            emit sigEndOfTrick();
        }
    }
    else
    {
        emit sigSelectPlayer(gameState.currentPlayer);
        emit sigPlayCard(gameState.currentPlayer);
    }
}
/*****************************************************************************/
void TarotEngine::BidSequence()
{
    if (gameState.Next() == true)
    {
        if (gameState.contract == PASS)
        {
            // All the players have passed, deal again new cards
            cptVuDonne = 0;
            gameState.sequence = Game::WAIT_PLAYER;
            emit sigDealAgain();
        }
        else
        {
            if (gameState.contract != GUARD_AGAINST)
            {
                deal.SetDogOwner(ATTACK);
            }
            else
            {
                // With this contract, the dog belongs to the defense
                deal.SetDogOwner(DEFENSE);
            }

            if (infos.contrat == GARDE_SANS || infos.contrat == GARDE_CONTRE)
            {
                // On n'affiche pas le chien et on commence la partie immédiatement
                sendDepartDonne();
                jeu();
                return;
            }
            else
            {
                cptVuChien = 0;
                gameState.sequence = WAIT_CHIEN; // Waiting for the taker's discard
                emit sigShowDog();
            }
        }
    }
    else
    {
        emit sigSelectPlayer(gameState.currentPlayer);
        emit sigAskBid(gameState.contract);
    }
}
/*****************************************************************************/
void TarotEngine::ShowDog()
{
    int i;
    Card *c;





    sendShowChien(); // Prise ou garde, on affiche le chien chez tout le monde
}
/*****************************************************************************/
/**
 * Use a custom deal for the game
 */
void TarotEngine::CustomDeal()
{
    int j;
    Card *c;
    quint8 params[NB_HAND_CARDS] = {0};   // cartes du joueur (3 joueurs: 24 cartes, 4j:18, 5j:15)



    QMapIterator<QTcpSocket *, Player *> i(players);
    while (i.hasNext())
    {
        i.next();
        i.value()->emptyDeck();
        Place p = i.value()->getPlace();
        Deck *d;

        if (p == SUD)
        {
            d = &editor.southDeck;
        }
        else if (p == EST)
        {
            d = &editor.eastDeck;
        }
        else if (p == OUEST)
        {
            d = &editor.westDeck;
        }
        else
        {
            d = &editor.northDeck;
        }

        for (j = 0; j < NB_HAND_CARDS; j++)
        {
            c = d->at(j);
            if (c->getType() == EXCUSE)
            {
                score.setExcuse(p);
            }
            c->setOwner(p);
            i.value()->addCard(c);
            params[j] = c->getId();
        }
        sendCards(p, params);
    }

    deckChien = editor.chienDeck;
    mainDeck.clear();
}
/*****************************************************************************/
void TarotEngine::CreateDeal()
{
    currentTrick.clear();

    if (dealType == RANDOM_DEAL)
    {
        DealFile editor;
        if (editor.LoadFile(dealFile) == true)
        {
            // SOUTH = 0, EAST = 1, NORTH = 2, WEST = 3,
            currentTrick.append(editor.southDeck);
            currentTrick.append(editor.eastDeck);
            currentTrick.append(editor.northDeck);
            currentTrick.append(editor.westDeck);
            currentTrick.append(editor.dogDeck);
        }
        else
        {
            // Fall back to default mode
            // FIXME: load a problem into the logging mechanism
            dealType = RANDOM_DEAL;
        }
    }

    if (dealType == RANDOM_DEAL)
    {
        dealNumber = qrand() % RAND_MAX;
        for (int i = 0; i < 78; i++)
        {
            currentTrick.append(TarotDeck::GetCard(i));
        }
        currentTrick.shuffle(dealNumber);
    }

    int n = 0;
    for (int i=0; i<Game.numberOfPlayers; i++)
    {
        players[i].GetDeck().clear();
        Place p = players[i].GetPlace();

        for (j = 0; j<Game.GetNumberOfCards(); j++)
        {
            int index = n * Game.GetNumberOfCards() + j;
            Card *c = currentTrick.at(index);
            c->SetOwner(p);
            players[i].GetDeck().append(c);
        }
        n++;
    }


    // Remaining cards go to the dog
    Deck dog = currentTrick.mid(n * Game.GetNumberOfCards());
    deal.SetDog(dog);
    currentTrick.clear();
}


//=============================================================================
// End of file TarotEngine.cpp
//=============================================================================
