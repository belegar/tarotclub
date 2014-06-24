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
    , mSequence(STOPPED)
    , mInitialized(false)
    , mConnected(false)
{

}
/*****************************************************************************/
void Client::Initialize()
{
    score.Reset();
    mSequence = STOPPED;

    if (!mInitialized)
    {
        mTcpClient.Create();
        mThread = std::thread(Client::EntryPoint, this);
        mInitialized = true;
    }
}
/*****************************************************************************/
Deck::Statistics &Client::GetStatistics()
{
    return stats;
}
/*****************************************************************************/
Deck &Client::GetCurrentTrick()
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
Deck &Client::GetMyDeck()
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
Score &Client::GetScore()
{
    return score;
}
/*****************************************************************************/
Place Client::GetPlace()
{
    return mPlace;
}
/*****************************************************************************/
void Client::SetMyIdentity(const Identity &ident)
{
    mIdentity = ident;
}
/*****************************************************************************/
void Client::SetDiscard(Deck &discard)
{
    mPlayer.RemoveDuplicates(discard);

    std::string message = "Dog: " + mDog.GetCardList();
    TLogInfo(message);
    TLogInfo("Player: " + mPlayer.GetCardList());
    TLogInfo("Discard: " + discard.GetCardList());

    // Send discard to the server
    SendDiscard(discard);
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
Deck Client::AutoDiscard()
{
    Deck discard = mDog;

    TLogInfo("Auto discard before: " + discard.GetCardList());

    // We're looking for trumps or kings in the dog and we replace
    // them by other valid cards
    for (Deck::ConstIterator iter = mDog.Begin(); iter != mDog.End(); ++iter)
    {
        Card *c = (*iter);
        if ((c->GetSuit() == Card::TRUMPS) ||
                ((c->GetSuit() != Card::TRUMPS) && (c->GetValue() == 14)))
        {
            // looking for valid card
            for (Deck::ConstIterator j = mPlayer.Begin(); j != mPlayer.End(); ++j)
            {
                Card *cdeck = (*j);
                if ((cdeck->GetSuit() != Card::TRUMPS) && (cdeck->GetValue() < 14))
                {
                    // Swap cards between the player and the discard
                    mPlayer.Remove(cdeck);
                    mPlayer.Append(c);
                    discard.Remove(c);
                    discard.Append(cdeck);
                }
            }
        }
    }

    TLogInfo("Auto discard after: " + discard.GetCardList());
    return discard;
}
/*****************************************************************************/
Card *Client::Play()
{
    Card *c = NULL;

    for (Deck::ConstIterator i = mPlayer.Begin(); i != mPlayer.End(); ++i)
    {
        c = (*i);
        if (IsValid(c) == true)
        {
            break;
        }
    }
    return c;
}
/*****************************************************************************/
bool Client::IsValid(Card *c)
{
    return mPlayer.CanPlayCard(c, currentTrick);
}
/*****************************************************************************/
bool Client::IsConnected()
{
    return mConnected;
}
/*****************************************************************************/
void Client::ConnectToHost(const std::string &hostName, std::uint16_t port)
{
    if (!mTcpClient.IsValid())
    {
        // Create a socket before connection
        mTcpClient.Create();
    }

    if (mTcpClient.Connect(hostName, port) == true)
    {
        mConnected = true;
        mQueue.Push(START);
    }
    else
    {
        TLogError("Client cannot connect to server.");
    }
}
/*****************************************************************************/
void Client::Close()
{
    mConnected = false;
    mTcpClient.Close();

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
    Client * pt = static_cast<Client*>(pthis);
    pt->Run();
}
/*****************************************************************************/
void Client::Run()
{
    std::string buffer;
    Command cmd;

    while(true)
    {
        mQueue.WaitAndPop(cmd);
        if (cmd == START)
        {
            while (mConnected)
            {
                std::int32_t ret = mTcpClient.Recv(buffer);
                if (ret > 0)
                {
                    ByteArray data(buffer);

                    std::vector<Protocol::PacketInfo> packets = Protocol::DecodePacket(data);

                    // Execute all packets
                    for (std::uint16_t i = 0U; i< packets.size(); i++)
                    {
                        Protocol::PacketInfo inf = packets[i];

                        ByteArray subArray = data.SubArray(inf.offset, inf.size);
                        if (DoAction(subArray) == false)
                        {
                            // Exit from thread
                            return;
                        }
                    }
                }
                else if (ret == 0)
                {
                    mConnected = false;
                }
                else
                {
                    // Error!
                    TLogError("Rev() socket read error.");
                }
            }
        }
        else if (cmd == EXIT)
        {
            return;
        }
    }
}
/*****************************************************************************/
bool Client::DoAction(const ByteArray &data)
{
    bool ret = true;
    ByteStreamReader in(data);

    // Jump over some header bytes
    in.Seek(3U);

    // Get the user id
    std::uint32_t uuid;
    in >> uuid;

    // Get the command
    std::uint8_t cmd;
    in >> cmd;

    switch (cmd)
    {
        case Protocol::SERVER_MESSAGE:
        {
            std::string message;
            in >> message;
            mEventHandler.Message(message);
            break;
        }

        case Protocol::ADMIN_DISCONNECT:
        {
            mTcpClient.Close();
            ret = false;
            break;
        }

        case Protocol::SERVER_REQUEST_IDENTITY:
        {
            std::uint32_t myUuid;
            std::uint8_t mode;

            in >> mPlace;
            in >> myUuid;
            in >> mNbPlayers;
            in >> mode;

            mGameMode = (Tarot::GameMode)mode;
            mPlayer.SetUuid(myUuid);
            SendIdentity();
            mEventHandler.AssignedPlace();
            break;
        }

        case Protocol::SERVER_PLAYERS_LIST:
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
            mEventHandler.PlayersList();
            break;
        }

        case Protocol::SERVER_SEND_CARDS:
        {
            mPlayer.Clear();
            in >> mPlayer;

            if (mPlayer.Size() == Tarot::NumberOfCardsInHand(mNbPlayers))
            {
                score.Reset();
                UpdateStatistics();
                mEventHandler.ReceiveCards();
                SendSyncCards();
            }
            else
            {
                TLogError("Wrong number of cards received!");
            }
            break;
        }

        case Protocol::SERVER_REQUEST_BID:
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

        case Protocol::SERVER_SHOW_PLAYER_BID:
        {
            std::uint8_t c, slam;
            Place p;

            in >> p;
            in >> c;
            in >> slam;
            mEventHandler.ShowBid(p, (slam == 1 ? true : false), (Contract)c);
            break;
        }

        case Protocol::SERVER_SHOW_DOG:
        {
            in >> mDog;
            mDog.Sort();
            mSequence = SHOW_DOG;
            mEventHandler.ShowDog();
            break;
        }

        case Protocol::SERVER_ASK_FOR_DISCARD:
        {
            mSequence = BUILD_DISCARD;
            mEventHandler.BuildDiscard();
            break;
        }

        case Protocol::SERVER_START_DEAL:
        {
            in >> mBid.taker;
            in >> mBid.contract;
            in >> mBid.slam;
            in >> mShuffle;

            currentTrick.Clear();
            mSequence = SYNC_START;
            mEventHandler.StartDeal();
            break;
        }

        case Protocol::SERVER_ASK_FOR_HANDLE:
        {
            mSequence = BUILD_HANDLE;
            mEventHandler.AskForHandle();
            break;
        }

        case Protocol::SERVER_SHOW_HANDLE:
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
            // Don't display the handle if we are the owner because we are in an other sequence (play card)
            if (p != mPlace)
            {
                mSequence = SHOW_HANDLE;
                mEventHandler.ShowHandle();
            }
            break;
        }

        case Protocol::SERVER_SHOW_CARD:
        {
            std::string name;
            Place player;

            in >> player;
            in >> name;

            currentTrick.Append(TarotDeck::GetCard(name));
            mSequence = SYNC_CARD;
            mEventHandler.ShowCard(player, name);
            break;
        }

        case Protocol::SERVER_PLAY_CARD:
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

        case Protocol::SERVER_NEW_DEAL:
        {
            mEventHandler.NewDeal();
            break;
        }

        case Protocol::SERVER_END_OF_TRICK:
        {
            Place winner;
            in >> winner;
            mSequence = SYNC_TRICK;
            mEventHandler.WaitTrick(winner);
            break;
        }

        case Protocol::SERVER_END_OF_DEAL:
        {
            in >> score;
            mSequence = SYNC_READY;
            mEventHandler.EndOfDeal();
            break;
        }

        case Protocol::SERVER_END_OF_GAME:
        {
            mEventHandler.EndOfGame();
            break;
        }

        default:
            std::string msg = mIdentity.name + ": Unknown packet received.";
            TLogInfo(msg);
            break;
    }
    return ret;
}
/*****************************************************************************/
void Client::SendIdentity()
{
    ByteArray packet = Protocol::ClientReplyIdentity(mIdentity, mPlayer.GetUuid());
    SendPacket(packet);
}
/*****************************************************************************/
void Client::SendChatMessage(const std::string &message)
{
    ByteArray packet = Protocol::ClientChatMessage(message, mPlayer.GetUuid());
    SendPacket(packet);
}
/*****************************************************************************/
void Client::SendReady()
{
    ByteArray packet = Protocol::ClientReady(mPlayer.GetUuid());
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
void Client::SendCard(Card *c)
{
    if (c != NULL)
    {
        ByteArray packet = Protocol::ClientCard(c->GetName(), mPlayer.GetUuid());
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
void Client::SendSyncTrick()
{
    mSequence = IDLE;
    currentTrick.Clear();
    SendPacket(Protocol::ClientSyncTrick(mPlayer.GetUuid()));
}
/*****************************************************************************/
void Client::SendSyncHandle()
{
    SendPacket(Protocol::ClientSyncHandle(mPlayer.GetUuid()));
}
/*****************************************************************************/
void Client::SendPacket(const ByteArray &packet)
{
    mTcpClient.Send(packet.ToSring());
}

//=============================================================================
// End of file Client.cpp
//=============================================================================
