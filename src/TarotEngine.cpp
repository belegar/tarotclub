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
#include "Tools.h"
#include "Log.h"

/*****************************************************************************/
TarotEngine::TarotEngine()
{
    shuffle.type = RANDOM_DEAL;
    shuffle.seed = 0;
    for (int i = 0; i < 5; i++)
    {
        players[i].SetPlace((Place)i);
    }
}
/*****************************************************************************/
TarotEngine::~TarotEngine()
{

}
/*****************************************************************************/
void TarotEngine::NewGame(Game::Mode mode)
{
    deal.Initialize();
    gameState.gameMode = mode;
}
/*****************************************************************************/
void TarotEngine::SetShuffle(const TarotEngine::Shuffle &s)
{
    shuffle = s;
}
/*****************************************************************************/
TarotEngine::Shuffle TarotEngine::GetShuffle()
{
    return shuffle;
}
/*****************************************************************************/
void TarotEngine::SetDiscard(Deck &discard)
{
    deal.SetDog(discard, ATTACK);
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
    cntSyncHandle = 0;
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
Contract TarotEngine::SetBid(Contract c, bool slam, Place p)
{
    if (c > gameState.contract)
    {
        gameState.contract = c;
        gameState.taker = p;
        gameState.slamAnnounced = slam;
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
        ret =  players[p].CanPlayCard(c, currentTrick);
    }

    return (ret);
}
/*****************************************************************************/
bool TarotEngine::HasCard(Card *c, Place p)
{
    return players[p].GetDeck().HasCard(c);
}
/*****************************************************************************/
void TarotEngine::NewDeal()
{
    // 1. Initialize internal states
    deal.NewDeal();
    gameState.NewDeal();

    // 2. Give cards to all players
    CreateDeal();
    SendSignal(SIG_SEND_CARDS);

    // 3. Start the bid sequence
    BidSequence();
}
/*****************************************************************************/
void TarotEngine::StartDeal()
{
    cntSyncStart = 0;
    gameState.StartDeal();
    SendSignal(SIG_START_DEAL);
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
            if (gameState.IsDealFinished() == true)
            {
                EndOfDeal();
            }
            else
            {
                gameState.sequence = Game::PLAY_TRICK;
                GameSateMachine();
            }
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
void TarotEngine::SyncHandle()
{
    if (gameState.sequence == Game::SYNC_HANDLE)
    {
        cntSyncHandle++;
        if (cntSyncHandle >= gameState.numberOfPlayers)
        {
            gameState.sequence = Game::PLAY_TRICK;
        }
    }
}
/*****************************************************************************/
void TarotEngine::GameSateMachine()
{
    if (gameState.Next() == true)
    {
        TLogInfo("----------------------------------------------------");

        // The current trick winner will begin the next trick
        gameState.currentPlayer = deal.SetTrick(currentTrick, gameState);
        currentTrick.clear();
        cntSyncTrick = 0;
        gameState.sequence = Game::SYNC_TRICK;
        SendSignal(SIG_END_OF_TRICK, (Place)gameState.currentPlayer);
    }
    else
    {
        QString message = "Turn: " + QString().setNum(gameState.trickCounter) + " player: " + Util::ToString(gameState.currentPlayer);
        TLogInfo(message);
        SendSignal(SIG_PLAY_CARD, (Place)gameState.currentPlayer);
    }
}
/*****************************************************************************/
void TarotEngine::EndOfDeal()
{
    if (deal.Calculate(gameState) == true)
    {
        gameState.sequence = Game::SYNC_READY;
    }
    else
    {
        gameState.sequence = Game::STOP;
    }

    QMap<Place, Identity> list;

    // Create a QMap list of players
    for (int i = 0; i < gameState.numberOfPlayers; i++)
    {
        list.insert((Place)i, players[i].GetIdentity());
    }
    deal.GenerateEndDealLog(gameState, list);

    cntSyncReady = 0;
    SendSignal(SIG_END_OF_DEAL);
}
/*****************************************************************************/
void TarotEngine::SendSignal(TarotEngine::Signal s, Place p, Contract c)
{
    SignalInfo signal;
    signal.sig = s;
    signal.p = p;
    signal.c = c;
    mSubject.Notify(signal);
}
/*****************************************************************************/
void TarotEngine::BidSequence()
{
    // If a slam has been announced, we start immediately the deal
    if ((gameState.Next() == true) ||
            (gameState.slamAnnounced == true))
    {
        if (gameState.contract == PASS)
        {
            // All the players have passed, deal again new cards
            cntSyncReady = 0;
            gameState.sequence = Game::SYNC_READY;
            SendSignal(SIG_DEAL_AGAIN);
        }
        else
        {
            if ((gameState.contract == GUARD_WITHOUT) || (gameState.contract == GUARD_AGAINST))
            {
                // No discard is made, set the owner of the dog
                if (gameState.contract != GUARD_AGAINST)
                {
                    deal.SetDogOwner(ATTACK);
                }
                else
                {
                    // Guard _against_, the dog belongs to the defense
                    deal.SetDogOwner(DEFENSE);
                }

                // We do not display the dog and start the deal immediatly
                StartDeal();
                return;
            }
            else
            {
                cntSyncDog = 0;
                gameState.sequence = Game::SYNC_DOG; // Waiting for the taker's discard
                SendSignal(SIG_SHOW_DOG);
            }
        }
    }
    else
    {
        SendSignal(SIG_REQUEST_BID, (Place)gameState.currentPlayer, (Contract)gameState.contract);
    }
}
/*****************************************************************************/
void TarotEngine::RegisterListener(Observer<TarotEngine::SignalInfo> &listener)
{
    mSubject.Attach(listener);
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
            shuffle.seed = qrand() % RAND_MAX;
        }
        currentTrick.Shuffle(shuffle.seed);
    }

    int n = gameState.GetNumberOfCards();
    for (int i = 0; i < gameState.numberOfPlayers; i++)
    {
        players[i].GetDeck().clear();
        players[i].GetDeck().append(currentTrick.mid(i * n, n));
        TLogInfo("Player " + Util::ToString((Place)i) + " deck: " + players[i].GetDeck().GetCardList());
    }

    // Remaining cards go to the dog
    Deck dog;
    dog.append(currentTrick.mid(gameState.numberOfPlayers * n));

    TLogInfo("Dog deck: " + dog.GetCardList());

    deal.SetDog(dog, NO_TEAM);
    currentTrick.clear();
}


//=============================================================================
// End of file TarotEngine.cpp
//=============================================================================
