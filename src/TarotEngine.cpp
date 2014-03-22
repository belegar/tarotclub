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

#include <chrono>
#include <random>

#include "TarotEngine.h"
#include "DealFile.h"
#include "Identity.h"
#include "Util.h"
#include "Log.h"

/*****************************************************************************/
TarotEngine::TarotEngine(IEvent &handler)
    : mEventHandler(handler)
{
    shuffle.type = Game::RANDOM_DEAL;
    shuffle.seed = 0U;
    for (std::uint32_t i = 0U; i < 5U; i++)
    {
        players[i].SetPlace(i);
    }
}
/*****************************************************************************/
TarotEngine::~TarotEngine()
{

}
/*****************************************************************************/
/**
 * @brief TarotEngine::Initialize
 * Call this method before clients connections
 */
void TarotEngine::Initialize()
{
    cntSyncIdentity = 0;
}
/*****************************************************************************/
void TarotEngine::NewGame(Game::Mode mode)
{
    deal.Initialize();
    gameState.gameMode = mode;
}
/*****************************************************************************/
void TarotEngine::SetShuffle(const Game::Shuffle &s)
{
    shuffle = s;
}
/*****************************************************************************/
Game::Shuffle TarotEngine::GetShuffle()
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

    if (handle.Size() == 10)
    {
        type = SIMPLE_HANDLE;
    }
    else if (handle.Size() == 13)
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
    currentTrick.Append(c);
    players[p.Value()].GetDeck().Remove(c);
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
        c = Contract::PASS;
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
Place TarotEngine::GetFreePlayer()
{
    Place p;

    // Look for free space
    for (std::uint32_t i = 0U; i < gameState.numberOfPlayers; i++)
    {
        if (players[i].IsFree() == true)
        {
            p = i;
            break;
        }
    }
    return p;
}
/*****************************************************************************/
/**
 * @brief TarotEngine::SetIdentity
 * @param uuid
 * @param ident
 * @return true if the game is full
 */
bool TarotEngine::SetIdentity(std::uint32_t uuid, const Identity &ident)
{
    Player *player = GetPlayer(uuid);
    if (player)
    {
        player->SetIdentity(ident);
        cntSyncIdentity++;
    }
    else
    {
        TLogError("Fatal error, Uuid not found!");
    }

    if (cntSyncIdentity == gameState.numberOfPlayers)
    {
        return true;
    }
    else
    {
        return false;
    }
}
/*****************************************************************************/
Player &TarotEngine::GetPlayer(Place p)
{
    return players[p.Value()];
}
/*****************************************************************************/
Player *TarotEngine::GetPlayer(std::uint32_t uuid)
{
    for (std::uint32_t i = 0U; i < gameState.numberOfPlayers; i++)
    {
        if (players[i].GetUuid() == uuid)
        {
            return &players[i];
        }
    }
    return NULL;
}
/*****************************************************************************/
Place TarotEngine::GetPlayerPlace(std::uint32_t uuid)
{
    Place p;

    for (std::uint32_t i = 0U; i < gameState.numberOfPlayers; i++)
    {
        if (players[i].GetUuid() == uuid)
        {
            p = i;
        }
    }
    return p;
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
        ret =  players[p.Value()].CanPlayCard(c, currentTrick);
    }

    return (ret);
}
/*****************************************************************************/
bool TarotEngine::HasCard(Card *c, Place p)
{
    return players[p.Value()].GetDeck().HasCard(c);
}
/*****************************************************************************/
void TarotEngine::NewDeal()
{
    // 1. Initialize internal states
    deal.NewDeal();
    gameState.NewDeal();

    // 2. Give cards to all players
    CreateDeal();
    mEventHandler.SendCards();

    // 3. Start the bid sequence
    BidSequence();
}
/*****************************************************************************/
void TarotEngine::StartDeal()
{
    cntSyncStart = 0;
    gameState.StartDeal();
    mEventHandler.StartDeal();
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
        currentTrick.Clear();
        cntSyncTrick = 0;
        gameState.sequence = Game::SYNC_TRICK;
        mEventHandler.EndOfTrick(gameState.currentPlayer);
    }
    else
    {
        std::stringstream message;

        message << "Turn: " << gameState.trickCounter << " player: " << gameState.currentPlayer.ToString();
        TLogInfo(message.str());
        mEventHandler.PlayCard(gameState.currentPlayer);
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

    std::map<Place, Identity> list;

    // Create a QMap list of players
    for (std::uint32_t i = 0U; i < gameState.numberOfPlayers; i++)
    {
        Place p(i);
        list[p] = players[i].GetIdentity();
    }
    deal.GenerateEndDealLog(gameState, list);

    cntSyncReady = 0;
    mEventHandler.EndOfDeal();
}
/*****************************************************************************/
void TarotEngine::BidSequence()
{
    // If a slam has been announced, we start immediately the deal
    if ((gameState.Next() == true) ||
            (gameState.slamAnnounced == true))
    {
        if (gameState.contract == Contract::PASS)
        {
            // All the players have passed, deal again new cards
            cntSyncReady = 0;
            gameState.sequence = Game::SYNC_READY;
            mEventHandler.DealAgain();
        }
        else
        {
            if ((gameState.contract == Contract::GUARD_WITHOUT) || (gameState.contract == Contract::GUARD_AGAINST))
            {
                // No discard is made, set the owner of the dog
                if (gameState.contract != Contract::GUARD_AGAINST)
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
                mEventHandler.ShowDog();
            }
        }
    }
    else
    {
        mEventHandler.RequestBid(gameState.contract, gameState.currentPlayer);
    }
}
/*****************************************************************************/
void TarotEngine::CreateDeal()
{
    currentTrick.Clear();

    if (shuffle.type == Game::CUSTOM_DEAL)
    {
        DealFile editor;
        if (editor.LoadFile(shuffle.file) == true)
        {
            // SOUTH = 0, EAST = 1, NORTH = 2, WEST = 3,
            currentTrick.Append(editor.GetSouthDeck());
            currentTrick.Append(editor.GetEastDeck());
            currentTrick.Append(editor.GetNorthDeck());
            currentTrick.Append(editor.GetWestDeck());
            currentTrick.Append(editor.GetDogDeck());
        }
        else
        {
            // Fall back to default mode
            // FIXME: load a problem into the logging mechanism
            shuffle.type = Game::RANDOM_DEAL;
        }
    }
    else
    {
        for (int i = 0; i < 78; i++)
        {
            currentTrick.Append(TarotDeck::GetCard(i));
        }

        if (shuffle.type == Game::RANDOM_DEAL)
        {
            shuffle.seed =  std::chrono::system_clock::now().time_since_epoch().count();
        }
        currentTrick.Shuffle(shuffle.seed);
    }

    int n = gameState.GetNumberOfCards();
    for (std::uint32_t i = 0U; i < gameState.numberOfPlayers; i++)
    {
        players[i].GetDeck().Clear();
        players[i].GetDeck().Append(currentTrick.Mid(i * n, n));

        std::stringstream message;
        Place p(i);
        message << "Player " << p.ToString() << " deck: " << players[i].GetDeck().GetCardList();
        TLogInfo(message.str());
    }

    // Remaining cards go to the dog
    Deck dog;
    dog.Append(currentTrick.Mid(gameState.numberOfPlayers * n));

    std::stringstream message;
    message << "Dog deck: " << dog.GetCardList();
    TLogInfo(message.str());

    deal.SetDog(dog, NO_TEAM);
    currentTrick.Clear();
}


//=============================================================================
// End of file TarotEngine.cpp
//=============================================================================
