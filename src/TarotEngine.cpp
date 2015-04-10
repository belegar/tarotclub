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
TarotEngine::TarotEngine(const std::string &i_dbName)
    : mDeal(mRemoteDb)
    , mDbName(i_dbName)
    , mNbPlayers(4U)
    , mSequence(STOPPED)
    , mPosition(0U)
    , mTrickCounter(0U)
    , mGameMode(Tarot::ONE_DEAL)
{
    std::chrono::system_clock::rep seed = std::chrono::system_clock::now().time_since_epoch().count(); // rep is long long
    mSeed = static_cast<std::uint32_t>(seed);

    for (std::uint8_t i = 0U; i < 5U; i++)
    {
        mHandleAsked[i] = false;
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
    mSequence = STOPPED;
    ResetAck();
    mShuffle.Initialize();
}
/*****************************************************************************/
void TarotEngine::CreateTable(std::uint8_t nbPlayers)
{
    // Save parameters
    mNbPlayers = nbPlayers;

    // 1. Initialize internal states
    mBid.Initialize();

    for (std::uint32_t i = 0U; i < 5U; i++)
    {
        mPlayers[i].SetUuid(0U); // close all the clients
    }
    mPlayersIdent.clear();

    // Choose the dealer
    std::default_random_engine generator(mSeed);
    std::uniform_int_distribution<std::uint32_t> distribution(0, nbPlayers - 1);
    mDealer = distribution(generator);

    // Wait for ready
    ResetAck();
    mSequence = WAIT_FOR_PLAYERS;
}
/*****************************************************************************/
void TarotEngine::NewGame(Tarot::GameMode mode, const Tarot::Shuffle &s, std::uint8_t numberOfTurns)
{
    mShuffle = s;
    mGameMode = mode;
    mScore.NewGame(numberOfTurns);
    mSequence = WAIT_FOR_READY;
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
/**
 * @brief TarotEngine::StartDeal
 * @return The first player to play
 */
Place TarotEngine::StartDeal()
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

    std::stringstream ss;
    ss << "Taker: " << mBid.taker.ToString() << " / ";
    ss << "Contract: " << mBid.contract.ToString();
    TLogInfo(ss.str());

    mDeal.StartDeal(mCurrentPlayer, mBid);

    return mCurrentPlayer;
}
/*****************************************************************************/
Tarot::Shuffle TarotEngine::GetShuffle()
{
    return mShuffle;
}
/*****************************************************************************/
Place TarotEngine::GetWinner()
{
    // FIXME: Not implemented

    return Place(Place::SOUTH);

//    if (mGameMode == Tarot::TOURNAMENT)
//    {
//        return mDeal.GetPodium();
    //    }
}
/*****************************************************************************/
Identity TarotEngine::GetIdentity(Place p)
{
    if (mPlayersIdent.find(p) == mPlayersIdent.end())
    {
        TLogError("Player does not exists");
    }
    return mPlayersIdent[p];
}
/*****************************************************************************/
bool TarotEngine::SetDiscard(const Deck &discard)
{
    Deck dog = mDeal.GetDog();
    bool valid = mPlayers[mBid.taker.Value()].TestDiscard(discard, dog, mNbPlayers);

    if (valid)
    {
        // Add the dog to the player's deck, and then filter the discard
        mPlayers[mBid.taker.Value()] += dog;
        mPlayers[mBid.taker.Value()].RemoveDuplicates(discard);

        std::stringstream ss;
        ss << "Received discard: " << discard.ToString() << " / ";
        ss << "Taker's deck after the discard: " << mPlayers[mBid.taker.Value()].ToString();
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
        Team team;

        if (p == mBid.taker)
        {
            team = ATTACK;
        }
        else
        {
            team = DEFENSE;
        }

        ResetAck();
        mSequence = WAIT_FOR_SHOW_HANDLE;
        mDeal.SetHandle(handle, team);
    }
    return valid;
}
/*****************************************************************************/
bool TarotEngine::SetCard(const Card &c, Place p)
{
    bool ret = false;

    if (mPlayers[p.Value()].CanPlayCard(c, currentTrick))
    {
        currentTrick.Append(c);
        mPlayers[p.Value()].Remove(c);

        std::stringstream ss;
        ss << "Player " << p.ToString() << " played " << c.GetName() << " Engine player deck is: " << mPlayers[p.Value()].ToString();
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
        ss << "The player cannot play the card: " << c.GetName()
           << " Engine deck is: " << mPlayers[p.Value()].ToString();

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
void TarotEngine::RemovePlayer(std::uint32_t uuid)
{
    for (std::uint32_t i = 0U; i < mNbPlayers; i++)
    {
        if (mPlayers[i].GetUuid() == uuid)
        {
            mPlayers[i].SetUuid(0U);
            // Remove its identity, index is the place
            mPlayersIdent.erase(Place(i));
        }
    }
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
    return AckFromAllPlayers();
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
Points TarotEngine::GetCurrentGamePoints()
{
    return mCurrentPoints;
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
        mCurrentPlayer = mDeal.SetTrick(currentTrick, mTrickCounter);
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
void TarotEngine::EndOfDeal(const std::string &tableName)
{
    mCurrentPoints.Clear();
    mDeal.AnalyzeGame(mCurrentPoints, mNbPlayers);
    mDeal.CalculateScore(mCurrentPoints);
    mDeal.GenerateEndDealLog(mPlayersIdent, tableName, mDbName);

    ResetAck();
    mSequence = WAIT_FOR_END_OF_DEAL;
}
/*****************************************************************************/
bool TarotEngine::NextDeal()
{
    bool continueGame = false;

    if (mGameMode == Tarot::TOURNAMENT)
    {
        continueGame = mScore.AddPoints(mCurrentPoints, mBid, mNbPlayers);
    }

    return continueGame;
}
/*****************************************************************************/

/**
 * @brief TarotEngine::BidSequence
 * @return The next sequence to go
 */
void TarotEngine::BidSequence()
{
    // If a slam has been announced, we start immediately the deal
    if (IsEndOfTrick() || mBid.slam)
    {
        if (mBid.contract == Contract::PASS)
        {
            // All the players have passed, deal again new cards
            ResetAck();
            mSequence = WAIT_FOR_ALL_PASSED;
        }
        else
        {
            if ((mBid.contract == Contract::GUARD_WITHOUT) || (mBid.contract == Contract::GUARD_AGAINST))
            {
                // No discard is made, set the owner of the dog
                if (mBid.contract != Contract::GUARD_AGAINST)
                {
                    mDeal.SetDiscard(mDeal.GetDog(), ATTACK);
                }
                else
                {
                    // Guard _against_, the dog belongs to the defense
                    mDeal.SetDiscard(mDeal.GetDog(), DEFENSE);
                }

                // We do not display the dog and start the deal immediatly
                ResetAck();
                mSequence = WAIT_FOR_START_DEAL;
            }
            else
            {
                // Show the dog to all the players
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
        if (!editor.LoadFile(mShuffle.file))
        {
            // Fall back to default mode
            TLogError("Cannot load custom deal file");
            mShuffle.type = Tarot::Shuffle::RANDOM_DEAL;
        }

        if (editor.IsValid(mNbPlayers))
        {
            // SOUTH = 0, EAST = 1, NORTH = 2, WEST = 3,
            currentTrick.Append(editor.GetSouthDeck());
            currentTrick.Append(editor.GetEastDeck());
            currentTrick.Append(editor.GetNorthDeck());
            currentTrick.Append(editor.GetWestDeck());
            currentTrick.Append(editor.GetDogDeck());

            // Override the current player
            mCurrentPlayer = editor.GetFirstPlayer();
            mDealer = mCurrentPlayer.Previous(mNbPlayers);
        }
        else
        {
            // Fall back to default mode
            TLogError("Invalid deal file");
            mShuffle.type = Tarot::Shuffle::RANDOM_DEAL;
        }
    }
    else
    {
        RandomDeal();
    }

    int n = Tarot::NumberOfCardsInHand(mNbPlayers);
    bool valid;
    do
    {
        valid = true;
        for (std::uint32_t i = 0U; i < mNbPlayers; i++)
        {
            mPlayers[i].Clear();
            mPlayers[i].Append(currentTrick.Mid(i * n, n));

            if (mPlayers[i].HasOnlyOneOfTrump())
            {
                RandomDeal(); // deal again
                valid = false;
                TLogInfo("Petit sec detected!");
            }

            std::stringstream message;
            Place p(i);
            message << "Player " << p.ToString() << " deck: " << mPlayers[i].ToString();
            TLogInfo(message.str());
        }
    }
    while (!valid);

    // Remaining cards go to the dog
    Deck dog(currentTrick.Mid(mNbPlayers * n));
    mDeal.SetDog(dog);

    std::stringstream message;
    message << "Dog deck: " << dog.ToString();
    TLogInfo(message.str());

    currentTrick.Clear();
}
/*****************************************************************************/
void TarotEngine::RandomDeal()
{
    currentTrick.CreateTarotDeck();
    if (mShuffle.type == Tarot::Shuffle::RANDOM_DEAL)
    {
        std::chrono::system_clock::rep seed = std::chrono::system_clock::now().time_since_epoch().count(); // rep is long long
        mShuffle.seed = static_cast<std::uint32_t>(seed);
    }
    currentTrick.Shuffle(mShuffle.seed);
}

//=============================================================================
// End of file TarotEngine.cpp
//=============================================================================
