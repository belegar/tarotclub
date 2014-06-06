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
    , mNbPlayers(4U)
    , mSequence(STOPPED)
{
    mSeed = std::chrono::system_clock::now().time_since_epoch().count();
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
    mSequence = STOPPED;
    ResetAck();
    mShuffle.Initialize();
}
/*****************************************************************************/
void TarotEngine::CreateGame(Tarot::GameMode mode, const Game::Shuffle &s, std::uint8_t nbPlayers)
{
    // Save parameters
    mShuffle = s;
    numberOfPlayers = nbPlayers;
    mGameMode = mode;

    // 1. Initialize internal states
    mDeal.Initialize();
    mBid.Initialize();
    mPosition = 0U;
    trickCounter = 0U;

    // Choose the dealer
    std::default_random_engine generator(mSeed);
    std::uniform_int_distribution<std::uint32_t> distribution(0, nbPlayers-1);
    mDealer = distribution(generator);

    // Wait for ready
    ResetAck();
    mSequence = WAIT_FOR_PLAYERS;
}
/*****************************************************************************/
void TarotEngine::NewDeal()
{
    // 1. Initialize internal states
    mDeal.NewDeal();
    mBid.Initialize();
    mPosition = 0U;
    trickCounter = 0U;

    // 2. Choose the dealer and the first player to start the bid
    mDealer = Tarot::NextPlayer(mDealer);
    mCurrentPlayer = Tarot::NextPlayer(mDealer, mNbPlayers); // The first player on the dealer's right begins the bid

    // 3. Give cards to all players
    CreateDeal();

    // 4. Prepare the wait for ack
    ResetAck();
    mSequence = WAIT_FOR_CARDS;
}
/*****************************************************************************/
void TarotEngine::StartDeal()
{
    trickCounter = 0;
    mPosition = 0;
    mBid = bid;

    // In case of slam, the first player to play is the taker.
    // Otherwise, it is the player on the right of the dealer
    if (mBid.slam == true)
    {
        mCurrentPlayer = mBid.place;
    }
    else
    {
        mCurrentPlayer = FindNextPlayer(mDealer); // The first player on the dealer's right
    }

    mEventHandler.StartDeal();
}
/*****************************************************************************/
Game::Shuffle TarotEngine::GetShuffle()
{
    return mShuffle;
}
/*****************************************************************************/
void TarotEngine::SetDiscard(Deck &discard)
{
    mDeal.SetDog(discard, ATTACK);
    // as soon as the discard has been done, the deal can start
    StartDeal();
}
/*****************************************************************************/
/**
 * @brief TarotEngine::SetHandle
 * @param handle
 * @param p
 * @return  true if the handle is valid, otherwise false
 */
bool TarotEngine::SetHandle(Deck &handle, Place p)
{
    /* FIXME: add protections
    1) L'origine du client (sud, est ... ip ??)
    2) La validité de la poignée (présence dans le deck du joueur, utilisation de l'excuse)
    3) La poignée doit être déclarée avant de jouer la première carte
    4) Puis envoyer la poignée à tout le monde ...
    */
    Team handleOwner;
    Handle type;
    bool valid = true;

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

    // FIXME: Add a handle validity test here

    if (valid)
    {
        if (p == mGame.GetTaker())
        {
            handleOwner = ATTACK;
            mGame.attackHandle.declared = true;
            mGame.attackHandle.type = type;
        }
        else
        {
            handleOwner = DEFENSE;
            mGame.defenseHandle.declared = true;
            mGame.defenseHandle.type = type;
        }

        mDeal.SetHandle(handle, handleOwner);
    }
    return valid;
}
/*****************************************************************************/
void TarotEngine::SetCard(Card *c, Place p)
{
    c->SetOwner(p);
    currentTrick.Append(c);
    mPlayers[p.Value()].Remove(c);
}
/*****************************************************************************/
Contract TarotEngine::SetBid(Contract c, bool slam, Place p)
{
    if (c > mBid.contract)
    {
        mBid.contract = c;
        mBid.place = p;
        mBid.slam = slam;
    }
    else
    {
        c = Contract::PASS;
    }

    // Advance sequence: show the declared bid to all the players
    ResetAck();
    mSequence = WAIT_FOR_SHOW_BID;
    return c;
}
/*****************************************************************************/
bool TarotEngine::AckFromAllPlayers()
{
    bool ack = false;
    std::uint8_t counter = 0U;

    for (std::uint8_t i = 0U; i < mNbPlayers; i++)
    {
        if (mPlayers[i].HasAck())
        {
            counter++;
        }
    }

    if (counter == mNbPlayers)
    {
        ack = true;
    }

    return ack;
}
/*****************************************************************************/
void TarotEngine::ResetAck()
{
    for (std::uint8_t i = 0U; i < mNbPlayers; i++)
    {
        mPlayers[i].SetAck(false);
    }
}
/*****************************************************************************/
void TarotEngine::StopGame()
{
    // FIXME: set the state
}
/*****************************************************************************/
/**
 * @brief TarotEngine::AddPlayer
 *
 * Adds a player around the table with the specified UUID. If the table is
 * full, the returned place is NOWHERE.
 *
 * @param uuid
 * @return
 */
Place TarotEngine::AddPlayer(std::uint32_t uuid)
{
    Place p = Place::NOWHERE;

    if (mSequence == WAIT_FOR_PLAYERS)
    {
        // Look for free space
        for (std::uint32_t i = 0U; i < mNbPlayers; i++)
        {
            if (mPlayers[i].IsFree() == true)
            {
                p = i;
                mPlayers[i].SetUuid(uuid);
                break;
            }
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
    if (mSequence == WAIT_FOR_PLAYERS)
    {
        Player *player = GetPlayer(uuid);
        if (player != NULL)
        {
            // Look for place
            Place p = GetPlayerPlace(uuid);
            mPlayersIdent[p] = ident;
            player->SetAck();
        }
    }

    bool full = AckFromAllPlayers();

    if (full)
    {
        mSequence = WAIT_FOR_READY;
    }
    return full;
}
/*****************************************************************************/
Player *TarotEngine::GetPlayer(Place p)
{
    Player *player = NULL;

    if (p < Place::NOWHERE)
    {
        player = &mPlayers[p.Value()];
    }

    return player;
}
/*****************************************************************************/
Player *TarotEngine::GetPlayer(std::uint32_t uuid)
{
    Player *player = NULL;
    for (std::uint8_t i = 0U; i < mNbPlayers; i++)
    {
        if (mPlayers[i].GetUuid() == uuid)
        {
            player = &mPlayers[i];
            break;
        }
    }
    return player;
}
/*****************************************************************************/
Place TarotEngine::GetPlayerPlace(std::uint32_t uuid)
{
    Place p = NOWHERE;

    for (std::uint8_t i = 0U; i < mNbPlayers; i++)
    {
        if (mPlayers[i].GetUuid() == uuid)
        {
            p = i;
        }
    }
    return p;
}
/*****************************************************************************/
Score &TarotEngine::GetScore()
{
    return mDeal.GetScore();
}
/*****************************************************************************/
Game &TarotEngine::GetGameInfo()
{
    return mGame;
}
/*****************************************************************************/
Deck TarotEngine::GetDog()
{
    return mDeal.GetDog();
}
/*****************************************************************************/
bool TarotEngine::IsCardValid(Card *c, Place p)
{
    bool ret = false;

    if (mGame.Sequence() == Game::BUILD_DOG)
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
    else if (mGame.Sequence() == Game::PLAY_TRICK)
    {
        ret =  mPlayers[p.Value()].CanPlayCard(c, currentTrick);
    }

    return (ret);
}
/*****************************************************************************/
bool TarotEngine::HasCard(Card *c, Place p)
{
    return mPlayers[p.Value()].HasCard(c);
}
/*****************************************************************************/
bool TarotEngine::SyncDog()
{
    bool finished = false;
    if (mGame.Sequence() == Game::SHOW_DOG)
    {
        finished = mSync.Add();
        if (finished)
        {
            mGame.NextSequence();
        }
    }
    return finished;
}
/*****************************************************************************/
void TarotEngine::SyncBid()
{
    if (mGame.Sequence == Game::BID)
    {
        if (mSync.Add())
        {
            BidSequence();
        }
    }
}
/*****************************************************************************/
void TarotEngine::SyncCard()
{
    if (mGame.sequence == Game::SYNC_CARD)
    {
        cntSyncCard++;
        if (cntSyncCard >= mGame.numberOfPlayers)
        {
            mGame.sequence = Game::PLAY_TRICK;
            GameSequence();
        }
    }
}
/*****************************************************************************/
void TarotEngine::SyncTrick()
{
    if (mGame.sequence == Game::SYNC_TRICK)
    {
        cntSyncTrick++;
        if (cntSyncTrick >= mGame.numberOfPlayers)
        {
            if (mGame.IsDealFinished() == true)
            {
                EndOfDeal();
            }
            else
            {
                mGame.sequence = Game::PLAY_TRICK;
                GameSequence();
            }
        }
    }
}
/*****************************************************************************/
bool TarotEngine::Sync(Sequence sequence, std::uint32_t uuid)
{
    bool ret = false;
    Player *player = GetPlayer(uuid);

    if (player != NULL)
    {
        if (mSequence == sequence)
        {
            player->SetAck();
            ret = AckFromAllPlayers();
        }
    }
    return ret;
}
/*****************************************************************************/
void TarotEngine::SyncHandle()
{
    if (mGame.Sequence() == Game::SHOW_HANDLE)
    {
        cntSyncHandle++;
        if (cntSyncHandle >= mGame.numberOfPlayers)
        {
            mGame.sequence = Game::PLAY_TRICK;
        }
    }
}
/*****************************************************************************/
void TarotEngine::GameSequence()
{
    // Choose the next player, if true the trick is finished
    if (mGame.NextPlayer() == true)
    {
        TLogInfo("----------------------------------------------------");

        // The current trick winner will begin the next trick
        mGame.currentPlayer = mDeal.SetTrick(currentTrick, mGame);
        currentTrick.Clear();
        mSync.Reset(SYNC_TRICK);
        mEventHandler.EndOfTrick(mGame.currentPlayer);
    }
    else
    {
        std::stringstream message;

        message << "Turn: " << mGame.trickCounter << " player: " << mGame.currentPlayer.ToString();
        TLogInfo(message.str());
        mEventHandler.PlayCard(mGame.currentPlayer);
    }
}
/*****************************************************************************/
void TarotEngine::EndOfDeal()
{
    // Result of the calculation is true if another deal is required
    if (mDeal.Calculate(mGame) == true)
    {
        mSync.Reset(SYNC_READY);
    }
    else
    {
        // The game is finished, no any other deal
        mGame.Stop();
        mSync.Reset(SYNC_STOP);
    }

    mDeal.GenerateEndDealLog(mGame);
    mEventHandler.EndOfDeal();
}
/*****************************************************************************/

/**
 * @brief TarotEngine::BidSequence
 * @return The next sequence to go
 */
TarotEngine::BidResult TarotEngine::BidSequence()
{
    BidResult res = NEXT_PLAYER;

    // If a slam has been announced, we start immediately the deal
    if ((NextPlayer() == true) ||
            (mBid.slam == true))
    {
        if (mBid.contract == Contract::PASS)
        {
            // All the players have passed, deal again new cards
            res = ALL_PASSED;
        }
        else
        {
            if ((mBid.contract == Contract::GUARD_WITHOUT) || (mBid.contract == Contract::GUARD_AGAINST))
            {
                // No discard is made, set the owner of the dog
                if (mBid.contract != Contract::GUARD_AGAINST)
                {
                    mDeal.SetDogOwner(ATTACK);
                }
                else
                {
                    // Guard _against_, the dog belongs to the defense
                    mDeal.SetDogOwner(DEFENSE);
                }

                // We do not display the dog and start the deal immediatly
                res = START_DEAL;
                ResetAck();
                mSequence = WAIT_FOR_START_DEAL;
                return;
            }
            else
            {
                // Show the dog to all the players
                res = SHOW_DOG;
                ResetAck();
                mSequence = WAIT_FOR_SHOW_DOG;
            }
        }
    }

    return res;
}
/*****************************************************************************/
bool TarotEngine::NextPlayer()
{
    bool endOfTrick = false;
    mPosition++;
    if (mPosition > numberOfPlayers)
    {
        mPosition = 0;
        trickCounter++;
        endOfTrick = true;
    }
    else
    {
        if (mPosition > 1)
        {
            mCurrentPlayer = FindNextPlayer(mCurrentPlayer);
        }
        endOfTrick = false;
    }
    return endOfTrick;
}
/*****************************************************************************/
void TarotEngine::CreateDeal()
{
    currentTrick.Clear();

    if (mShuffle.type == Game::CUSTOM_DEAL)
    {
        DealFile editor;
        if (editor.LoadFile(mShuffle.file) == true)
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
            TLogError("Cannot load custom deal file");
            mShuffle.type = Game::RANDOM_DEAL;
        }
    }
    else
    {
        for (int i = 0; i < 78; i++)
        {
            currentTrick.Append(TarotDeck::GetCard(i));
        }

        if (mShuffle.type == Game::RANDOM_DEAL)
        {
            mShuffle.seed =  std::chrono::system_clock::now().time_since_epoch().count();
        }
        currentTrick.Shuffle(mShuffle.seed);
    }

    int n = mGame.GetNumberOfCards();
    for (std::uint32_t i = 0U; i < mGame.numberOfPlayers; i++)
    {
        mPlayers[i].Clear();
        mPlayers[i].Append(currentTrick.Mid(i * n, n));

        std::stringstream message;
        Place p(i);
        message << "Player " << p.ToString() << " deck: " << mPlayers[i].GetCardList();
        TLogInfo(message.str());
    }

    // Remaining cards go to the dog
    Deck dog;
    dog.Append(currentTrick.Mid(mGame.numberOfPlayers * n));

    std::stringstream message;
    message << "Dog deck: " << dog.GetCardList();
    TLogInfo(message.str());

    mDeal.SetDog(dog, NO_TEAM);
    currentTrick.Clear();
}


//=============================================================================
// End of file TarotEngine.cpp
//=============================================================================
