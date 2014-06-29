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
TarotEngine::TarotEngine()
    : mNbPlayers(4U)
    , mSequence(STOPPED)
{
	std::chrono::system_clock::rep seed = std::chrono::system_clock::now().time_since_epoch().count(); // rep is long long
	mSeed = static_cast<std::uint32_t>(seed);
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
void TarotEngine::CreateGame(Tarot::GameMode mode, const Tarot::Shuffle &s, std::uint8_t nbPlayers)
{
    // Save parameters
    mShuffle = s;
    mNbPlayers = nbPlayers;
    mGameMode = mode;

    // 1. Initialize internal states
    mDeal.Initialize();
    mBid.Initialize();

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

    // 2. Choose the dealer and the first player to start the bid
    mDealer = mDealer.Next(mNbPlayers);
    mCurrentPlayer = mDealer.Next(mNbPlayers); // The first player on the dealer's right begins the bid

    // 3. Give cards to all players
    CreateDeal();

    // 4. Prepare the wait for ack
    ResetAck();
    mSequence = WAIT_FOR_CARDS;
}
/*****************************************************************************/
void TarotEngine::StartDeal()
{
    mTrickCounter = 0U;
    mPosition = 0U;

    for (std::uint8_t i = 0U; i < 5U; i++)
    {
        mHandleAsked[i] = false;
    }

    // In case of slam, the first player to play is the taker.
    // Otherwise, it is the player on the right of the dealer
    if (mBid.slam == true)
    {
        mCurrentPlayer = mBid.taker;
    }
    else
    {
        mCurrentPlayer = mDealer.Next(mNbPlayers); // The first player on the dealer's right
    }
}
/*****************************************************************************/
Tarot::Shuffle TarotEngine::GetShuffle()
{
    return mShuffle;
}
/*****************************************************************************/
bool TarotEngine::SetDiscard(const Deck &discard)
{
    bool valid = mPlayers[mBid.taker.Value()].TestDiscard(discard, mDog, mNbPlayers);

    if (valid)
    {
        // Add the dog to the player's deck, and then filter the discard
        mPlayers[mBid.taker.Value()] += mDog;
        mPlayers[mBid.taker.Value()].RemoveDuplicates(discard);

        std::stringstream ss;
        ss << "Taker's deck after the discard: " << mPlayers[mBid.taker.Value()].GetCardList();
        TLogInfo(ss.str());

        mDeal.SetDiscard(discard, ATTACK);
        ResetAck();
        mSequence = WAIT_FOR_START_DEAL;
    }
    return valid;
}
/*****************************************************************************/
/**
 * @brief TarotEngine::SetHandle
 * @param handle
 * @param p
 * @return  true if the handle is valid, otherwise false
 */
bool TarotEngine::SetHandle(const Deck &handle, Place p)
{
    bool valid = mPlayers[p.Value()].TestHandle(handle);

    if (valid)
    {
        std::uint8_t type;
        Team handleOwner;

        if (handle.Size() == 10U)
        {
            type = Tarot::Handle::SIMPLE;
        }
        else if (handle.Size() == 13U)
        {
            type = Tarot::Handle::DOUBLE;
        }
        else
        {
            type = Tarot::Handle::TRIPLE;
        }

        if (p == mBid.taker)
        {
            handleOwner = ATTACK;
            mAttackHandle.declared = true;
            mAttackHandle.type = type;
        }
        else
        {
            handleOwner = DEFENSE;
            mDefenseHandle.declared = true;
            mDefenseHandle.type = type;
        }

        mDeal.SetHandle(handle, handleOwner);
    }
    return valid;
}
/*****************************************************************************/
bool TarotEngine::SetCard(Card *c, Place p)
{
    bool ret = false;

    if (mPlayers[p.Value()].CanPlayCard(c, currentTrick))
    {
        c->SetOwner(p);
        currentTrick.Append(c);
        mPlayers[p.Value()].Remove(c);

        std::stringstream ss;
        ss << "Player " << p.ToString() << " played " << c->GetName() << " Engine player deck is: " << mPlayers[p.Value()].GetCardList();
        TLogInfo(ss.str());

        // ------- PREPARE NEXT ONE
        mPosition++; // done for this player
        mCurrentPlayer = mCurrentPlayer.Next(mNbPlayers); // next player!
        ResetAck();
        mSequence = WAIT_FOR_SHOW_CARD;
        ret = true;
    }
    else
    {
        std::stringstream ss;
        ss << "The player cannot play the card: " << c->GetName()
           << " Engine deck is: " << mPlayers[p.Value()].GetCardList();

        TLogError(ss.str());
    }
    return ret;
}
/*****************************************************************************/
Contract TarotEngine::SetBid(Contract c, bool slam, Place p)
{
    if (c > mBid.contract)
    {
        mBid.contract = c;
        mBid.taker = p;
        mBid.slam = slam;
    }
    else
    {
        c = Contract::PASS;
    }

    // ------- PREPARE NEXT ONE
    mPosition++; // done for this player
    mCurrentPlayer = mCurrentPlayer.Next(mNbPlayers); // next player!
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
    mSequence = STOPPED;
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
    Place p = Place::NOWHERE;

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
/**
 * @brief TarotEngine::GameSequence
 * @return true if the trick is finished
 */
void TarotEngine::GameSequence()
{
    // If end of trick, prepare next one
    if (IsEndOfTrick())
    {
        TLogInfo("----------------------------------------------------\n");

        // The current trick winner will begin the next trick
        mCurrentPlayer = mDeal.SetTrick(currentTrick, mBid, mTrickCounter);
        currentTrick.Clear();
        ResetAck();
        mSequence = WAIT_FOR_END_OF_TRICK;
    }
    // Special case of first round: players can declare a handle
    else if ((mTrickCounter == 0U) &&
            (!mHandleAsked[mCurrentPlayer.Value()]))
    {
        mHandleAsked[mCurrentPlayer.Value()] = true;
        mSequence = WAIT_FOR_HANDLE;
    }
    else
    {
        std::stringstream message;

        message << "Turn: " << (std::uint32_t)mTrickCounter << " player: " << mCurrentPlayer.ToString();
        TLogInfo(message.str());

        ResetAck();
        mSequence = WAIT_FOR_PLAYED_CARD;
    }
}
/*****************************************************************************/
void TarotEngine::EndOfDeal()
{
    bool continueGame = false;
    mDeal.AnalyzeGame(mNbPlayers);
    mDeal.CalculateScore(mBid, mAttackHandle, mDefenseHandle);
    if (mGameMode == Tarot::TOURNAMENT)
    {
        continueGame = mDeal.AddScore(mBid, mNbPlayers);
    }

    ResetAck();
    // Result of the calculation is true if another deal is required
    if (continueGame == true)
    {
        mSequence = WAIT_FOR_READY;
    }
    else
    {
        // The game is finished, no any other deal
        mSequence = WAIT_FOR_READY;
    }

    mDeal.GenerateEndDealLog(mBid, mPlayersIdent);
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
    if (IsEndOfTrick() || mBid.slam)
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
                    mDeal.SetDiscard(mDog, ATTACK);
                }
                else
                {
                    // Guard _against_, the dog belongs to the defense
                    mDeal.SetDiscard(mDog, DEFENSE);
                }

                // We do not display the dog and start the deal immediatly
                res = START_DEAL;
                ResetAck();
                mSequence = WAIT_FOR_START_DEAL;
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
    else
    {
        ResetAck();
        mSequence = WAIT_FOR_BID;
    }

    return res;
}
/*****************************************************************************/
void TarotEngine::DiscardSequence()
{
    mSequence = WAIT_FOR_DISCARD;
}
/*****************************************************************************/
bool TarotEngine::IsEndOfTrick()
{
    bool endOfTrick = false;
    if (mPosition >= mNbPlayers)
    {
        // Trick as ended, all the players have played
        mPosition = 0U;
        mTrickCounter++;
        endOfTrick = true;
    }
    return endOfTrick;
}
/*****************************************************************************/
void TarotEngine::CreateDeal()
{
    currentTrick.Clear();

    if (mShuffle.type == Tarot::Shuffle::CUSTOM_DEAL)
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
            mShuffle.type = Tarot::Shuffle::RANDOM_DEAL;
        }
    }
    else
    {
        for (int i = 0; i < 78; i++)
        {
            currentTrick.Append(TarotDeck::GetCard(i));
        }

        if (mShuffle.type == Tarot::Shuffle::RANDOM_DEAL)
        {
			std::chrono::system_clock::rep seed = std::chrono::system_clock::now().time_since_epoch().count(); // rep is long long
            mShuffle.seed = static_cast<std::uint32_t>(seed);
        }
        currentTrick.Shuffle(mShuffle.seed);
    }

    int n = Tarot::NumberOfCardsInHand(mNbPlayers);
    for (std::uint32_t i = 0U; i < mNbPlayers; i++)
    {
        mPlayers[i].Clear();
        mPlayers[i].Append(currentTrick.Mid(i * n, n));

        std::stringstream message;
        Place p(i);
        message << "Player " << p.ToString() << " deck: " << mPlayers[i].GetCardList();
        TLogInfo(message.str());
    }

    // Remaining cards go to the dog
    mDog.Clear();
    mDog.Append(currentTrick.Mid(mNbPlayers * n));

    std::stringstream message;
    message << "Dog deck: " << mDog.GetCardList();
    TLogInfo(message.str());

    currentTrick.Clear();
}


//=============================================================================
// End of file TarotEngine.cpp
//=============================================================================
