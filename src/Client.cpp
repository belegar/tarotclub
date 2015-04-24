/*=============================================================================
 * TarotClub - Client.cpp
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

#include "Client.h"
#include "Identity.h"
#include "Log.h"

/*****************************************************************************/
Client::Client(IEvent &handler)
    : mEventHandler(handler)
    , mGameMode(Tarot::ONE_DEAL)
    , mNbPlayers(4U)
    , mSequence(STOPPED)
    , mInitialized(false)
    , mConnected(false)
{

}
/*****************************************************************************/
void Client::Initialize()
{
    mSequence = STOPPED;
    mPlayersIdent.clear();
    mPlace = Place::NOWHERE;

    if (!mInitialized)
    {
        mInitialized = mTcpClient.Start();
        if (mInitialized)
        {
            mThread = std::thread(Client::EntryPoint, this);
        }
        else
        {
            TLogError("Cannot initialize socket client");
        }
    }
}
/*****************************************************************************/
Deck::Statistics &Client::GetStatistics()
{
    return stats;
}
/*****************************************************************************/
Deck Client::GetCurrentTrick()
{
    return currentTrick;
}
/*****************************************************************************/
Deck &Client::GetDogDeck()
{
    return mDog;
}
/*****************************************************************************/
Deck &Client::GetHandleDeck()
{
    return handleDeck;
}
/*****************************************************************************/
Deck Client::GetMyDeck()
{
    return mPlayer;
}
/*****************************************************************************/
bool Client::TestHandle(const Deck &handle)
{
    return mPlayer.TestHandle(handle);
}
/*****************************************************************************/
bool Client::TestDiscard(const Deck &discard)
{
    return mPlayer.TestDiscard(discard, mDog, mNbPlayers);
}
/*****************************************************************************/
void Client::SetDiscard(const Deck &discard)
{
    mPlayer += mDog;
    mPlayer.RemoveDuplicates(discard);
}
/*****************************************************************************/
void Client::SetPlayedCard(const Card &c)
{
    mPlayer.Remove(c);
}
/*****************************************************************************/
void Client::SetMyDeck(const Deck &deck)
{
    mPlayer.Clear();
    mPlayer.Append(deck);
}
/*****************************************************************************/
Points Client::GetPoints()
{
    return mPoints;
}
/*****************************************************************************/
Place Client::GetPlace()
{
    return mPlace;
}
/*****************************************************************************/
std::string Client::GetTablePlayerName(Place p)
{
    std::string name;
    if (mPlayersIdent.find(p) != mPlayersIdent.end())
    {
        name = mPlayersIdent[p].name;
    }
    return name;
}
/*****************************************************************************/
void Client::SetMyIdentity(const Identity &ident)
{
    mIdentity = ident;
}
/*****************************************************************************/
Contract Client::CalculateBid()
{
    int total = 0;
    Contract cont;

    UpdateStatistics();

    // Set points according to the card values
    if (stats.bigTrump == true)
    {
        total += 9;
    }
    if (stats.fool == true)
    {
        total += 7;
    }
    if (stats.littleTrump == true)
    {
        if (stats.trumps == 5)
        {
            total += 5;
        }
        else if (stats.trumps == 6 || stats.trumps == 7)
        {
            total += 7;
        }
        else if (stats.trumps > 7)
        {
            total += 8;
        }
    }

    // Each trump is 1 point
    // Each major trump is 1 more point
    total += stats.trumps * 2;
    total += stats.majorTrumps * 2;
    total += stats.kings * 6;
    total += stats.queens * 3;
    total += stats.knights * 2;
    total += stats.jacks;
    total += stats.weddings;
    total += stats.longSuits * 5;
    total += stats.cuts * 5;
    total += stats.singletons * 3;
    total += stats.sequences * 4;

    // We can decide the bid
    if (total <= 35)
    {
        cont = Contract::PASS;
    }
    else if (total >= 36  && total <= 50)
    {
        cont = Contract::TAKE;
    }
    else if (total >= 51  && total <= 65)
    {
        cont = Contract::GUARD;
    }
    else if (total >= 66  && total <= 75)
    {
        cont = Contract::GUARD_WITHOUT;
    }
    else
    {
        cont = Contract::GUARD_AGAINST;
    }
    return cont;
}
/*****************************************************************************/
void Client::UpdateStatistics()
{
    stats.Reset();
    mPlayer.AnalyzeTrumps(stats);
    mPlayer.AnalyzeSuits(stats);
}
/*****************************************************************************/
Card Client::Play()
{
    Card c;

    for (Deck::ConstIterator it = mPlayer.Begin(); it != mPlayer.End(); ++it)
    {
        c = (*it);
        if (IsValid(c) == true)
        {
            break;
        }
    }
    return c;
}
/*****************************************************************************/
bool Client::IsValid(const Card &c)
{
    return mPlayer.CanPlayCard(c, currentTrick);
}
/*****************************************************************************/
Deck Client::AutoDiscard()
{
    Deck discard = mPlayer.AutoDiscard(mDog, mNbPlayers);
    mPlayer.RemoveDuplicates(discard);
    TLogInfo("Auto discard: " + discard.ToString());
    return discard;
}
/*****************************************************************************/
bool Client::IsConnected()
{
    return mConnected;
}
/*****************************************************************************/
void Client::Disconnect()
{
    mConnected = false;
    mTcpClient.Close();
}
/*****************************************************************************/
void Client::ConnectToHost(const std::string &hostName, std::uint16_t port)
{
    Disconnect();
    if (!mTcpClient.IsValid())
    {
        // Create a socket before connection
        mTcpClient.Create();
    }

    mHostName = hostName;
    mTcpPort = port;
    mQueue.Push(START);
}
/*****************************************************************************/
void Client::Close()
{
    Disconnect();

    if (mInitialized)
    {
        mQueue.Push(EXIT);
        mThread.join();
        mInitialized = false;
    }
}
/*****************************************************************************/
void Client::EntryPoint(void *pthis)
{
    Client *pt = static_cast<Client *>(pthis);
    pt->Run();
}
/*****************************************************************************/
void Client::Run()
{
    std::string buffer;
    Command cmd;

    while (true)
    {
        mQueue.WaitAndPop(cmd);
        if (cmd == START)
        {
            if (mTcpClient.Connect(mHostName, mTcpPort) == true)
            {
                mConnected = true;
                while (mConnected)
                {
                    std::int32_t ret = mTcpClient.Recv(buffer);
                    if (ret > 0)
                    {
                      //  ByteArray data(buffer);
                      //  mConnected = Protocol::DataManagement(this, data);

                        mProtocolQueue.Push(buffer); // Add packet to the queue
                        Protocol::GetInstance().Execute(this); // Actually decode the packet
                    }
                    else if (ret == 0)
                    {
                        mConnected = false;
                        TLogNetwork("Lost connection! Place: " + mPlace.ToString());
                    }
                    else
                    {
                        // try again to read from the socket
                    }
                }
                mPlayer.SetUuid(Protocol::INVALID_UID);
                mEventHandler.Error(IEvent::ErrDisconnectedFromServer);
            }
            else
            {
                TLogError("Client cannot connect to server.");
                mEventHandler.Error(IEvent::ErrCannotConnectToServer);
            }
        }
        else if (cmd == EXIT)
        {
            return;
        }
    }
}
/*****************************************************************************/
ByteArray Client::GetPacket()
{
    ByteArray data;
    if (!mProtocolQueue.TryPop(data))
    {
        TLogError("Lobby: work item called without any data in the queue!");
    }
    return data;
}
/*****************************************************************************/
bool Client::DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data)
{
    (void) src_uuid;
    (void) dest_uuid;
    bool ret = true;
    ByteStreamReader in(data);

    std::stringstream dbg;
    dbg << "Client command received: 0x" << std::hex << (int)cmd;
    TLogNetwork(dbg.str());

    switch (cmd)
    {

    case Protocol::LOBBY_CHAT_MESSAGE:
    {
        std::string message;
        in >> message;
        mEventHandler.LobbyMessage(message);
        break;
    }

    case Protocol::LOBBY_DISCONNECT:
    {
        mTcpClient.Close();
        mConnected = false;
        ret = false;
        break;
    }

    case Protocol::LOBBY_REQUEST_LOGIN:
    {
        std::uint32_t myUuid;
        in >> myUuid;
        mPlayer.SetUuid(myUuid);
        SendIdentity();
        break;
    }

    case Protocol::LOBBY_LOGIN_RESULT:
    {
        bool accepted;
        in >> accepted;
        if (accepted)
        {
            std::uint32_t tableSize;
            in >> tableSize;

            mTableList.clear();
            for (std::uint32_t i = 0U; i < tableSize; i++)
            {
                std::string name;
                std::uint32_t id;

                in >> name;
                in >> id;
                mTableList[name] = id;
            }
            mEventHandler.EnteredLobby();
        }
        else
        {
            mEventHandler.Error(IEvent::ErrLobbyAccessRefused);
            TLogError("Lobby has rejected the login.");
        }
        break;
    }

    case Protocol::LOBBY_PLAYERS_LIST:
    {
        std::uint8_t size;
        in >> size;

        mLobbyUsers.clear();
        for (int i = 0; i < size; i++)
        {
            std::string name;
            std::uint32_t uuid;

            in >> uuid;
            in >> name;
            mLobbyUsers[uuid] = name;
        }
        mEventHandler.LobbyPlayersList();
        break;
    }

    case Protocol::ADMIN_GAME_FULL:
    {
        bool full;
        in >> full;
        mEventHandler.AdminGameFull();
        break;
    }
    case Protocol::TABLE_ERROR_FULL:
    {
        mEventHandler.Error(IEvent::ErrTableFull);
        break;
    }

    case Protocol::TABLE_CHAT_MESSAGE:
    {
        std::string message;
        in >> message;
        mEventHandler.TableMessage(message);
        break;
    }

    case Protocol::TABLE_JOIN_REPLY:
    {
        bool status;
        std::uint32_t tableId;
        in >> tableId;
        in >> status;
        in >> mPlace;
        in >> mNbPlayers;
        if (status)
        {
            mEventHandler.TableJoinEvent(tableId);
        }
        else
        {
            mEventHandler.Error(IEvent::ErrTableAccessRefused);
        }
        break;
    }

    case Protocol::TABLE_QUIT_EVENT:
    {
        std::uint32_t tableId;
        in >> tableId;

        Initialize();
        mEventHandler.TableQuitEvent(tableId);
        break;
    }

    case Protocol::TABLE_PLAYERS_LIST:
    {
        std::uint8_t size;
        in >> size;

        mPlayersIdent.clear();
        for (int i = 0; i < size; i++)
        {
            Identity ident;
            Place place;

            in >> place;
            in >> ident;
            mPlayersIdent[place] = ident;
        }
        mEventHandler.TablePlayersList();
        break;
    }

    case Protocol::TABLE_NEW_GAME:
    {
        std::uint8_t mode;
        in >> mode;
        in >> mShuffle;
        in >> mNumberOfTurns;

        mGameMode = (Tarot::GameMode)mode;
        mEventHandler.NewGame();
        break;
    }

    case Protocol::TABLE_NEW_DEAL:
    {
        mPlayer.Clear();
        in >> mPlayer;

        if (mPlayer.Size() == Tarot::NumberOfCardsInHand(mNbPlayers))
        {
            mPoints.Clear();
            UpdateStatistics();
            mEventHandler.NewDeal();
            SendSyncCards();
        }
        else
        {
            TLogError("Wrong number of cards received!");
        }
        break;
    }

    case Protocol::TABLE_REQUEST_BID:
    {
        std::uint8_t c;
        Place p;

        in >> c; // Most important contract announced before
        in >> p;
        mEventHandler.SelectPlayer(p);
        if (p == mPlace)
        {
            // The request bid is for us! We must declare something
            mEventHandler.RequestBid((Contract)c);
        }
        break;
    }

    case Protocol::TABLE_SHOW_PLAYER_BID:
    {
        std::uint8_t c, slam;
        Place p;

        in >> p;
        in >> c;
        in >> slam;
        mEventHandler.ShowBid(p, (slam == 1 ? true : false), (Contract)c);
        break;
    }

    case Protocol::TABLE_ALL_PASSED:
    {
        mEventHandler.AllPassed();
        break;
    }

    case Protocol::TABLE_SHOW_DOG:
    {
        in >> mDog;
        mDog.Sort("TDSCH");
        mSequence = SHOW_DOG;
        mEventHandler.ShowDog();
        break;
    }

    case Protocol::TABLE_ASK_FOR_DISCARD:
    {
        mSequence = BUILD_DISCARD;
        mEventHandler.BuildDiscard();
        break;
    }

    case Protocol::TABLE_START_DEAL:
    {
        Place first;
        in >> first;
        in >> mBid.taker;
        in >> mBid.contract;
        in >> mBid.slam;
        in >> mShuffle;

        currentTrick.Clear();
        UpdateStatistics(); // cards in hand can have changed due to the dog
        mSequence = SYNC_START;
        mEventHandler.StartDeal();
        break;
    }

    case Protocol::TABLE_ASK_FOR_HANDLE:
    {
        mSequence = BUILD_HANDLE;
        mEventHandler.AskForHandle();
        break;
    }

    case Protocol::TABLE_SHOW_HANDLE:
    {
        Place p;
        in >> p;
        in >> handleDeck;
        if (mBid.taker == p)
        {
            handleDeck.SetOwner(ATTACK);
        }
        else
        {
            handleDeck.SetOwner(DEFENSE);
        }
        mSequence = SHOW_HANDLE;
        mEventHandler.ShowHandle();
        break;
    }

    case Protocol::TABLE_SHOW_CARD:
    {
        std::string name;
        Place player;

        in >> player;
        in >> name;

        currentTrick.Append(Card(name));
        mSequence = SYNC_CARD;
        mEventHandler.ShowCard(player, name);
        break;
    }

    case Protocol::TABLE_PLAY_CARD:
    {
        Place p;
        in >> p;

        mEventHandler.SelectPlayer(p);
        if (p == mPlace)
        {
            // Our turn to play a card
            mSequence = PLAY_TRICK;
            mEventHandler.PlayCard();
        }
        break;
    }

    case Protocol::TABLE_END_OF_TRICK:
    {
        Place winner;
        in >> winner;
        mSequence = SYNC_TRICK;
        mEventHandler.WaitTrick(winner);
        break;
    }

    case Protocol::TABLE_END_OF_DEAL:
    {
        in >> mPoints;

        mSequence = SHOW_SCORE;
        mEventHandler.EndOfDeal();
        break;
    }

    case Protocol::TABLE_END_OF_GAME:
    {
        Place winner;
        in >> winner;
        mEventHandler.EndOfGame(winner);
        break;
    }

    default:
        std::string msg = mIdentity.nickname + ": Unknown packet received.";
        TLogInfo(msg);
        break;
    }
    return ret;
}
/*****************************************************************************/
void Client::AdminNewGame(Tarot::GameMode gameMode, const Tarot::Shuffle &shuffle, std::uint8_t numberOfTurns)
{
    ByteArray packet = Protocol::AdminNewGame(gameMode, shuffle, numberOfTurns, mPlayer.GetUuid());
    SendPacket(packet);
}
/*****************************************************************************/
void Client::SendJoinTable(std::uint32_t tableId)
{
    ByteArray packet = Protocol::ClientJoinTable(mPlayer.GetUuid(), tableId);
    SendPacket(packet);
}
/*****************************************************************************/
void Client::SendQuitTable(std::uint32_t tableId)
{
    ByteArray packet = Protocol::ClientQuitTable(mPlayer.GetUuid(), tableId);
    SendPacket(packet);
}
/*****************************************************************************/
void Client::SendIdentity()
{
    ByteArray packet = Protocol::ClientReplyLogin(mPlayer.GetUuid(), mIdentity);
    SendPacket(packet);
}
/*****************************************************************************/
void Client::SendTableMessage(const std::string &message)
{
    ByteArray packet = Protocol::ClientTableMessage(message, mPlayer.GetUuid());
    SendPacket(packet);
}
/*****************************************************************************/
void Client::SendLobbyMessage(const std::string &message)
{
    ByteArray packet = Protocol::ClientLobbyMessage(message, mPlayer.GetUuid());
    SendPacket(packet);
}
/*****************************************************************************/
void Client::SendSyncNewGame()
{
    ByteArray packet = Protocol::ClientSyncNewGame(mPlayer.GetUuid());
    SendPacket(packet);
}
/*****************************************************************************/
void Client::SendError()
{
    ByteArray packet = Protocol::ClientError(mPlayer.GetUuid());
    SendPacket(packet);
}
/*****************************************************************************/
void Client::SendBid(Contract c, bool slam)
{
    ByteArray packet = Protocol::ClientBid(c, slam, mPlayer.GetUuid());
    SendPacket(packet);
}
/*****************************************************************************/
void Client::SendDiscard(const Deck &discard)
{
    ByteArray packet = Protocol::ClientDiscard(discard, mPlayer.GetUuid());
    SendPacket(packet);
}
/*****************************************************************************/
void Client::SendHandle(const Deck &handle)
{
    SendPacket(Protocol::ClientHandle(handle, mPlayer.GetUuid()));
}
/*****************************************************************************/
void Client::SendCard(const Card &c)
{
    if (c.IsValid())
    {
        ByteArray packet = Protocol::ClientCard(c.GetName(), mPlayer.GetUuid());
        SendPacket(packet);
    }
    else
    {
        SendError();
    }
}
/*****************************************************************************/
void Client::SendSyncDog()
{
    mSequence = IDLE;

    ByteArray packet = Protocol::ClientSyncDog(mPlayer.GetUuid());
    SendPacket(packet);
}
/*****************************************************************************/
void Client::SendSyncStart()
{
    mSequence = IDLE;
    SendPacket(Protocol::ClientSyncStart(mPlayer.GetUuid()));
}
/*****************************************************************************/
void Client::SendSyncShowCard()
{
    mSequence = IDLE;
    SendPacket(Protocol::ClientSyncShowCard(mPlayer.GetUuid()));
}
/*****************************************************************************/
void Client::SendSyncCards()
{
    mSequence = IDLE;
    SendPacket(Protocol::ClientSyncCards(mPlayer.GetUuid()));
}
/*****************************************************************************/
void Client::SendSyncBid()
{
    mSequence = IDLE;
    SendPacket(Protocol::ClientSyncBid(mPlayer.GetUuid()));
}
/*****************************************************************************/
void Client::SendSyncAllPassed()
{
    mSequence = IDLE;
    SendPacket(Protocol::ClientSyncAllPassed(mPlayer.GetUuid()));
}
/*****************************************************************************/
void Client::SendSyncTrick()
{
    mSequence = IDLE;
    currentTrick.Clear();
    SendPacket(Protocol::ClientSyncTrick(mPlayer.GetUuid()));
}
/*****************************************************************************/
void Client::SendSyncEndOfDeal()
{
    mSequence = IDLE;
    SendPacket(Protocol::ClientSyncEndOfDeal(mPlayer.GetUuid()));
}
/*****************************************************************************/
void Client::SendSyncHandle()
{
    SendPacket(Protocol::ClientSyncHandle(mPlayer.GetUuid()));
}
/*****************************************************************************/
void Client::SendPacket(const ByteArray &packet)
{
    std::uint8_t cmd = packet.Get(Protocol::COMMAND_OFFSET);
    std::stringstream dbg;
    dbg << "Client sending packet: 0x" << std::hex << (int)cmd;
    TLogNetwork(dbg.str());

    if (IsConnected())
    {
        mTcpClient.Send(packet.ToSring());
    }
    else
    {
        TLogNetwork("WARNING! try to send packet without any connection.");
    }
}

//=============================================================================
// End of file Client.cpp
//=============================================================================
