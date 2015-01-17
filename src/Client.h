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

#include <thread>
#include <string>
#include <map>
#include "TcpClient.h"
#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "Protocol.h"
#include "Common.h"
#include "Score.h"
#include "Deal.h"
#include "ThreadQueue.h"

/*****************************************************************************/
class Client : public Protocol::IWorkItem
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
        static const std::uint32_t ErrDisconnectedFromServer    = 6000U;
        static const std::uint32_t ErrCannotConnectToServer     = 6001U;

        virtual void Error(std::uint32_t errorId) = 0;
        virtual void EnteredLobby() = 0;
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

    // Getters
    Deck::Statistics &GetStatistics();
    Deck GetCurrentTrick();
    Deck &GetDogDeck();
    Deck &GetHandleDeck();
    Deck GetMyDeck();
    Deal GetDeal();
    Place GetPlace();
    std::uint8_t GetNumberOfPlayers()
    {
        return mNbPlayers;
    }
    std::map<Place, Identity> GetTablePlayersList()
    {
        return mPlayersIdent;
    }

    std::map<std::uint32_t, std::string> GetLobbyPlayersList()
    {
        return mLobbyUsers;
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
    std::uint32_t GetUuid() { return mPlayer.GetUuid(); }

    std::map<std::string, std::uint32_t>  GetTableList() { return mTableList; }

    std::string GetTablePlayerName(Place p);

    // Setters
    void SetMyIdentity(const Identity &ident);
    void SetDiscard(const Deck &discard);
    void SetPlayedCard(const Card &c);
    void SetMyDeck(const Deck &deck);

    // Network
    void ConnectToHost(const std::string &hostName, std::uint16_t port);
    bool IsConnected();
    void Disconnect();
    void Close();

    // Protocol methods
    void AdminNewGame(Tarot::GameMode gameMode, const Tarot::Shuffle &shuffle, std::uint8_t numberOfTurns);
    void SendJoinTable(std::uint32_t tableId);
    void SendQuitTable(std::uint32_t tableId);
    void SendBid(Contract c, bool slam);
    void SendSyncDog();
    void SendDiscard(const Deck &discard);
    void SendSyncNewGame();
    void SendError();
    void SendCard(const Card &c);
    void SendSyncTrick();
    void SendHandle(const Deck &handle);
    void SendSyncStart();
    void SendSyncShowCard();
    void SendSyncCards();
    void SendSyncBid();
    void SendSyncAllPassed();
    void SendSyncHandle();
    void SendSyncEndOfDeal();
    void SendTableMessage(const std::string &message);
    void SendLobbyMessage(const std::string &message);

private:
    enum Command
    {
        START,
        EXIT
    };

    IEvent     &mEventHandler;

    Player      mPlayer;    // Player's deck, utilities and assigned UUID
    Place       mPlace;     // assigned place around the table
    Identity    mIdentity;  // Player's identity

    // Memorized game states and parameters
    Tarot::GameMode mGameMode;
    std::map<Place, Identity> mPlayersIdent;  // players around the table
    std::map<std::uint32_t, std::string> mLobbyUsers; // pair of uuid, names
    std::map<std::string, std::uint32_t> mTableList;
    std::uint8_t mNbPlayers;
    Tarot::Bid  mBid;
    Tarot::Shuffle mShuffle;

    Deal        mDeal;
    Deck        mDog;
    Deck        handleDeck;     // declared poignee by a player
    Deck        currentTrick;

    Sequence    mSequence;
    Deck::Statistics   stats;   // statistics on player's cards

    TcpClient   mTcpClient;
    std::thread mThread;
    bool        mInitialized;
    ThreadQueue<Command> mQueue;
    bool        mConnected;
    std::string mHostName;
    std::uint16_t mTcpPort;

    static void EntryPoint(void *pthis);
    void Run();

    // From Protocol::WorkItem
    bool DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data);
    ByteArray GetPacket();

    // TarotClub Protocol methods
    void SendIdentity();
    void SendPacket(const ByteArray &packet);

};

#endif // CLIENT_H

//=============================================================================
// End of file Client.h
//=============================================================================
