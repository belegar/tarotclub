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

#ifndef _CLIENT_H
#define _CLIENT_H

#include <thread>
#include <string>
#include <map>
#include "TcpClient.h"
#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "TarotDeck.h"
#include "Protocol.h"
#include "Common.h"
#include "Score.h"
#include "Deal.h"
#include "ThreadQueue.h"

/*****************************************************************************/
class Client
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
        SYNC_READY
    };


    class IEvent
    {
    public:
        virtual void Message(const std::string &message) = 0;
        virtual void AssignedPlace() = 0;
        virtual void PlayersList() = 0;
        virtual void NewDeal() = 0;
        virtual void ReceiveCards() = 0;
        virtual void SelectPlayer(Place p) = 0;
        virtual void RequestBid(Contract highestBid) = 0;
        virtual void ShowBid(Place p, bool slam, Contract c) = 0;
        virtual void StartDeal() = 0;
        virtual void ShowDog() = 0;
        virtual void AskForHandle() = 0;
        virtual void ShowHandle() = 0;
        virtual void BuildDiscard() = 0;
        virtual void PlayCard() = 0;
        virtual void ShowCard(Place p, const std::string &name) = 0;
        virtual void WaitTrick(Place winner) = 0;
        virtual void EndOfDeal() = 0;
        virtual void EndOfGame() = 0;
    };

    Client(IEvent &handler);

    // Helpers
    void Initialize();
    bool TestHandle(const Deck &handle);
    bool TestDiscard(const Deck &discard);
    Contract CalculateBid();
    void UpdateStatistics();
    Card *Play();
    bool IsValid(Card *c);
    bool IsConnected();
    Deck AutoDiscard();

    // Getters
    Deck::Statistics &GetStatistics();
    Deck &GetCurrentTrick();
    Deck &GetDogDeck();
    Deck &GetHandleDeck();
    Deck &GetMyDeck();
    Score &GetScore();
    Place GetPlace();
    std::uint8_t GetNumberOfPlayers()
    {
        return mNbPlayers;
    }
    std::map<Place, Identity> GetPlayersList()
    {
        return mPlayersIdent;
    }
    Tarot::GameMode GetGameMode()
    {
        return mGameMode;
    }
    Sequence GetSequence()
    {
        return mSequence;
    }
    Tarot::Bid      GetBid()
    {
        return mBid;
    }
    Tarot::Shuffle GetShuffle()
    {
        return mShuffle;
    }

    // Setters
    void SetMyIdentity(const Identity &ident);
    void SetDiscard(const Deck &discard);

    // Network
    void ConnectToHost(const std::string &hostName, std::uint16_t port);
    void Close();

    // Protocol methods
    void SendBid(Contract c, bool slam);
    void SendSyncDog();
    void SendDiscard(const Deck &discard);
    void SendReady();
    void SendError();
    void SendCard(Card *c);
    void SendSyncTrick();
    void SendHandle(const Deck &handle);
    void SendSyncStart();
    void SendSyncShowCard();
    void SendSyncCards();
    void SendSyncBid();
    void SendSyncHandle();
    void SendChatMessage(const std::string &message);

private:
    enum Command
    {
        START,
        EXIT
    };

    Player      mPlayer;    // Player's deck, utilities and assigned UUID
    Place       mPlace;     // assigned place around the table
    Identity    mIdentity;  // Player's identity

    // Memorized game states and parameters
    Tarot::GameMode mGameMode;
    std::map<Place, Identity> mPlayersIdent;
    std::uint8_t mNbPlayers;
    Tarot::Bid  mBid;
    Tarot::Shuffle mShuffle;

    Score       score;
    Deck        mDog;
    Deck        handleDeck;     // declared poignee by a player
    Deck        currentTrick;
    IEvent     &mEventHandler;
    Sequence    mSequence;
    Deck::Statistics   stats;   // statistics on player's cards

    TcpClient   mTcpClient;
    std::thread mThread;
    bool        mInitialized;
    ThreadQueue<Command> mQueue;
    bool        mConnected;

    static void EntryPoint(void *pthis);
    void Run();

    // TarotClub Protocol methods
    void SendIdentity();
    void SendPacket(const ByteArray &packet);
    bool DoAction(const ByteArray &data);

};

#endif // _CLIENT_H

//=============================================================================
// End of file Client.h
//=============================================================================
