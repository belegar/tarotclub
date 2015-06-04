/*=============================================================================
 * TarotClub - Client.h
 *=============================================================================
 * This class manages the network protocol and is used to join a online game
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

#ifndef CLIENT_H
#define CLIENT_H


#include <string>
#include <map>

#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "Protocol.h"
#include "Common.h"
#include "Score.h"

/*****************************************************************************/
struct Client : public Protocol::IWorkItem
{

public:
    enum Sequence
    {
        STOPPED,
        IDLE,
        SHOW_DOG,
        BUILD_DISCARD,
        BUILD_HANDLE,
        SYNC_START,
        SHOW_HANDLE,
        SYNC_CARD,
        PLAY_TRICK,
        SYNC_TRICK,
        SYNC_READY,
        SHOW_SCORE
    };

    class IEvent
    {
    public:
        static const std::uint32_t ErrLobbyAccessRefused        = 4000U;
        static const std::uint32_t ErrTableAccessRefused        = 5000U;
        static const std::uint32_t ErrTableFull                 = 5001U;

        virtual void Error(std::uint32_t errorId) = 0;
        virtual void RequestLogin() = 0;
        virtual void EnteredLobby() = 0;
        virtual void KickedFromLobby() = 0;
        virtual void AdminGameFull() = 0;
        virtual void TableQuitEvent(std::uint32_t tableId) = 0;
        virtual void TableMessage(const std::string &message) = 0;
        virtual void LobbyMessage(const std::string &message) = 0;
        virtual void TableJoinEvent(std::uint32_t tableId) = 0;
        virtual void TablePlayersList() = 0;
        virtual void LobbyPlayersList() = 0;
        virtual void NewGame() = 0;
        virtual void NewDeal() = 0;
        virtual void SelectPlayer(Place p) = 0;
        virtual void RequestBid(Contract highestBid) = 0;
        virtual void ShowBid(Place p, bool slam, Contract c) = 0;
        virtual void AllPassed() = 0;
        virtual void StartDeal() = 0;
        virtual void ShowDog() = 0;
        virtual void AskForHandle() = 0;
        virtual void ShowHandle() = 0;
        virtual void BuildDiscard() = 0;
        virtual void PlayCard() = 0;
        virtual void ShowCard(Place p, const std::string &name) = 0;
        virtual void WaitTrick(Place winner) = 0;
        virtual void EndOfDeal() = 0;
        virtual void EndOfGame(Place winner) = 0;
    };

    // Memorized game states and parameters
    Player      mPlayer;    // Player's deck, utilities and assigned UUID
    Place       mPlace;     // assigned place around the table
    Identity    mIdentity;  // Player's identity
    Tarot::Game mGame;
    std::map<Place, std::uint32_t> mTablePlayers;  // pair of Place, uuid (players around the table)
    std::map<std::uint32_t, Identity> mLobbyUsers; // pair of uuid, identity
    std::map<std::string, std::uint32_t> mTables; // pair of tables name, id
    std::uint8_t mNbPlayers;
    Tarot::Bid  mBid;
    Tarot::Distribution mShuffle;
    Points      mPoints;
    std::string mResult;
    Deck        mDog;
    Deck        handleDeck;     // declared poignee by a player
    Deck        currentTrick;
    std::uint32_t mTableId;
    Sequence    mSequence;
    Deck::Statistics   mStats;   // statistics on player's cards

    Client(IEvent &handler);

    // Helpers
    void Initialize();
    bool TestHandle(const Deck &handle);
    bool TestDiscard(const Deck &discard);
    Contract CalculateBid();
    void UpdateStatistics();
    Card Play();
    bool IsValid(const Card &c);
    Deck AutoDiscard();

private:
    IEvent     &mEventHandler;

    // From Protocol::WorkItem
    bool DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data);
};



#endif // CLIENT_H

//=============================================================================
// End of file Client.h
//=============================================================================
