/*=============================================================================
 * TarotClub - TarotEngine.h
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
#ifndef TAROT_ENGINE_H
#define TAROT_ENGINE_H

#include "Card.h"
#include "Deck.h"
#include "Deal.h"
#include "Player.h"
#include "Common.h"
#include "ServerConfig.h"
#include "Observer.h"
#ifndef DESKTOP_PROJECT
#include "CouchDb.h"
#endif
#include "DummyRemoteDb.h"

/*****************************************************************************/
class TarotEngine
{
public:
    enum Sequence
    {
        STOPPED,
        WAIT_FOR_PLAYERS,
        WAIT_FOR_READY,
        WAIT_FOR_CARDS,
        WAIT_FOR_BID,
        WAIT_FOR_SHOW_BID,
        WAIT_FOR_ALL_PASSED,
        WAIT_FOR_SHOW_DOG,
        WAIT_FOR_DISCARD,
        WAIT_FOR_START_DEAL,
        WAIT_FOR_HANDLE,
        WAIT_FOR_SHOW_HANDLE,
        WAIT_FOR_PLAYED_CARD,
        WAIT_FOR_SHOW_CARD,
        WAIT_FOR_END_OF_TRICK,
        WAIT_FOR_END_OF_DEAL
    };

    TarotEngine(const std::string &i_dbName);
    ~TarotEngine();

    // Helpers
    void Initialize();
    void StopGame();
    void CreateTable(std::uint8_t nbPlayers);
    void NewGame(Tarot::GameMode mode, const Tarot::Shuffle &s, std::uint8_t numberOfTurns);
    void NewDeal();
    Place StartDeal();
    void EndOfDeal(const std::string &tableName);
    bool NextDeal();

    Place AddPlayer(std::uint32_t uuid);
    void RemovePlayer(std::uint32_t uuid);
    void BidSequence();
    void DiscardSequence();
    void GameSequence();
    bool Sync(Sequence sequence, std::uint32_t uuid);

    // Getters
    Player *GetPlayer(Place p);
    Player *GetPlayer(std::uint32_t uuid);
    Place GetPlayerPlace(std::uint32_t uuid);
    Place GetWinner();
    Identity GetIdentity(Place p);
    Place GetCurrentPlayer()
    {
        return mCurrentPlayer;
    }
    Sequence GetSequence()
    {
        return mSequence;
    }
    Points GetCurrentGamePoints();
    std::map<Place, Identity> GetPlayersList()
    {
        return mPlayersIdent;
    }
    std::uint8_t    GetNbPlayers()
    {
        return mNbPlayers;
    }
    Tarot::Bid      GetBid()
    {
        return mBid;
    }
    Tarot::GameMode GetGameMode()
    {
        return mGameMode;
    }
    Deck GetDog()
    {
        return mDeal.GetDog();
    }
    Tarot::Shuffle GetShuffle();

    bool IsLastTrick()
    {
        return Tarot::IsDealFinished(mTrickCounter, mNbPlayers);
    }

    // Setters
    bool SetIdentity(std::uint32_t uuid, const Identity &ident);
    bool SetDiscard(const Deck &discard);
    bool SetHandle(const Deck &handle, Place p);
    bool SetCard(const Card &c, Place p);
    Contract SetBid(Contract c, bool slam, Place p);

private:
    Player  mPlayers[5];     // [3..5] deck of players with their UUID, index = Place
    std::map<Place, Identity> mPlayersIdent;
    Deck    currentTrick;   // store the current trick cards played

#ifdef DESKTOP_PROJECT
    DummyRemoteDb mRemoteDb;
#else
    CouchDb mRemoteDb;
#endif
    Deal    mDeal;
    Score   mScore;
    Points  mCurrentPoints;
    std::string    mDbName; ///< Where finished deals are stored

    // Game state variables
    std::uint8_t    mNbPlayers;
    Sequence        mSequence;
    Tarot::Bid      mBid;
    Tarot::Shuffle  mShuffle;
    std::uint8_t    mPosition;          // Current position, [1..numberOfPlayers]
    Place           mDealer;            // who has dealt the cards
    std::uint8_t    mTrickCounter;       // number of tricks played [1..18] for 4 players
    Place           mCurrentPlayer;
    unsigned        mSeed;
    Tarot::GameMode mGameMode;
    bool            mHandleAsked[5U];

    bool AckFromAllPlayers();
    void ResetAck();
    void CreateDeal();
    bool IsEndOfTrick();
};

#endif // TAROT_ENGINE_H

//=============================================================================
// End of file TarotEngine.h
//=============================================================================
