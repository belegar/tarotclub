/*=============================================================================
 * TarotClub - TarotEngine.cpp
 *=============================================================================
 * Main Tarot engine 
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
    shuffle.type = RANDOM_DEAL;
    shuffle.seed = 0;
}
/*****************************************************************************/
TarotEngine::~TarotEngine()
{

}
/*****************************************************************************/
void TarotEngine::NewGame(Game::Mode mode)
{
    gameState.gameMode = mode;
}
/*****************************************************************************/
void TarotEngine::SetShuffle(TarotEngine::Shuffle &s)
{
    shuffle = s;
}
/*****************************************************************************/
void TarotEngine::SetDiscard(Deck &discard)
{
    deal.SetDog(discard);
    // as soon as the discard has been done, the deal can start
    StartDeal();
}
/*****************************************************************************/
void TarotEngine::SetHandle(Deck &handle, Place p)
{
    /* FIXME: add protections
    1) L'origine du client (sud, est ... ip ??)
    2) La validité de la poignée (présence dans le deck du joueur, utilisation de l'excuse)
    3) La poignée doit être déclarée avant de jouer la première carte
    4) Puis envoyer la poignée à tout le monde ...
    */
    Team handleOwner;
    Handle type;

    if (handle.size() == 10)
    {
        type = SIMPLE_HANDLE;
    }
    else if (handle.size() == 13)
    {
        type = DOUBLE_HANDLE;
    }
    else
    {
        type = TRIPLE_HANDLE;
    }

    if (p == gameState.taker)
    {
        handleOwner = ATTACK;
        gameState.attackHandle.declared = true;
        gameState.attackHandle.type = type;
    }
    else
    {
        handleOwner = DEFENSE;
        gameState.defenseHandle.declared = true;
        gameState.defenseHandle.type = type;
    }

    deal.SetHandle(handle, handleOwner);
}
/*****************************************************************************/
void TarotEngine::SetCard(Card *c, Place p)
{
    c->SetOwner(p);
    currentTrick.append(c);
    players[p].GetDeck().removeAll(c);
    cntSyncCard = 0;
    gameState.sequence = Game::SYNC_CARD;
}
/*****************************************************************************/
Contract TarotEngine::SetBid(Contract c, Place p)
{
    if (c > gameState.contract)
    {
        gameState.contract = c;
        gameState.taker = p;
    }
    else
    {
        c = PASS;
    }
    cntSyncBid = 0;
    gameState.sequence = Game::SYNC_BID;
    return c;
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
    return shuffle.seed;
}
/*****************************************************************************/
Game &TarotEngine::GetGameInfo()
{
    return gameState;
}
/*****************************************************************************/
Deal &TarotEngine::GetDeal()
{
    return deal;
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
        ret =  players[p].CanPlayCard(c, currentTrick, gameState);
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
    // 1. Initialize internal states
    deal.NewDeal();
    gameState.NewDeal();

    // 2. Give cards to all players
    CreateDeal();
    emit sigSendCards();

    // 3. Start the bid sequence
    BidSequence();
}
/*****************************************************************************/
void TarotEngine::StartDeal()
{
    cntSyncStart = 0;
    gameState.StartDeal();
    emit sigStartDeal();
}
/*****************************************************************************/
bool TarotEngine::SyncDog()
{
    bool finished = false;
    if (gameState.sequence == Game::SYNC_DOG)
    {
        cntSyncDog++;
        if (cntSyncDog >= gameState.numberOfPlayers)
        {
            finished = true;
            gameState.sequence = Game::BUILD_DOG;
        }
    }
    return finished;
}
/*****************************************************************************/
void TarotEngine::SyncStart()
{
    if (gameState.sequence == Game::SYNC_START)
    {
        cntSyncStart++;
        if (cntSyncStart >= gameState.numberOfPlayers)
        {
            gameState.sequence = Game::PLAY_TRICK;
            GameSateMachine();
        }
    }
}
/*****************************************************************************/
void TarotEngine::SyncBid()
{
    if (gameState.sequence == Game::SYNC_BID)
    {
        cntSyncBid++;
        if (cntSyncBid >= gameState.numberOfPlayers)
        {
            gameState.sequence = Game::BID;
            BidSequence();
        }
    }
}
/*****************************************************************************/
void TarotEngine::SyncCard()
{
    if (gameState.sequence == Game::SYNC_CARD)
    {
        cntSyncCard++;
        if (cntSyncCard >= gameState.numberOfPlayers)
        {
            gameState.sequence = Game::PLAY_TRICK;
            GameSateMachine();
        }
    }
}
/*****************************************************************************/
void TarotEngine::SyncTrick()
{
    if (gameState.sequence == Game::SYNC_TRICK)
    {
        cntSyncTrick++;
        if (cntSyncTrick >= gameState.numberOfPlayers)
        {
            gameState.sequence = Game::PLAY_TRICK;
            GameSateMachine();
        }
    }
}
/*****************************************************************************/
void TarotEngine::SyncReady()
{
    if (gameState.sequence == Game::SYNC_READY)
    {
        cntSyncReady++;
        if (cntSyncReady >= gameState.numberOfPlayers)
        {
            gameState.sequence = Game::DEAL;
            NewDeal();
        }
    }
}
/*****************************************************************************/
void TarotEngine::GameSateMachine()
{
    if (gameState.Next() == true)
    {
        // The current trick winner will begin the next trick
        gameState.currentPlayer = CalculateTrickWinner();
        deal.SetTrick(currentTrick, gameState.trickCounter);
        currentTrick.clear();

        if (gameState.IsDealFinished() == true)
        {
            deal.Calculate(gameState);
            dealCounter++;

            // Manage tournaments
            if ((dealCounter<MAX_ROUNDS) && (gameState.gameMode == Game::TOURNAMENT))
            {
                 gameState.sequence = Game::SYNC_READY;
            }
            else
            {
                 gameState.sequence = Game::STOP;
            }

            cntSyncReady = 0;
            emit sigEndOfDeal();
        }
        else
        {
            cntSyncTrick = 0;
            gameState.sequence = Game::SYNC_TRICK;
            emit sigEndOfTrick(gameState.currentPlayer);
        }
    }
    else
    {
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
            cntSyncReady = 0;
            gameState.sequence = Game::SYNC_READY;
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

            if ((gameState.contract == GUARD_WITHOUT) || (gameState.contract == GUARD_AGAINST))
            {
                // We do not display the dog and start the deal immediatly
                StartDeal();
                return;
            }
            else
            {
                cntSyncDog = 0;
                gameState.sequence = Game::SYNC_DOG; // Waiting for the taker's discard
                emit sigShowDog();
            }
        }
    }
    else
    {
        emit sigRequestBid(gameState.contract, gameState.currentPlayer);
    }
}
/*****************************************************************************/
void TarotEngine::CreateDeal()
{
    currentTrick.clear();

    if (shuffle.type == CUSTOM_DEAL)
    {
        DealFile editor;
        if (editor.LoadFile(shuffle.file) == true)
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
            shuffle.type = RANDOM_DEAL;
        }
    }

    if (shuffle.type != CUSTOM_DEAL)
    {
        for (int i = 0; i < 78; i++)
        {
            currentTrick.append(TarotDeck::GetCard(i));
        }

        if (shuffle.type == RANDOM_DEAL)
        {
            currentTrick.Shuffle(qrand() % RAND_MAX);
        }
        else
        {
            currentTrick.Shuffle(shuffle.seed);
        }
    }

    int n = 0;
    for (int i=0; i<gameState.numberOfPlayers; i++)
    {
        players[i].GetDeck().clear();
        Place p = players[i].GetPlace();

        for (int j = 0; j<gameState.GetNumberOfCards(); j++)
        {
            int index = n * gameState.GetNumberOfCards() + j;
            Card *c = currentTrick.at(index);
            c->SetOwner(p);
            players[i].GetDeck().append(c);
        }
        n++;
    }

    // Remaining cards go to the dog
    Deck dog;
    dog.append(currentTrick.mid(n * gameState.GetNumberOfCards()));
    deal.SetDog(dog);
    currentTrick.clear();
}


//=============================================================================
// End of file TarotEngine.cpp
//=============================================================================
