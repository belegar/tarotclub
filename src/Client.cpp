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
#include "defines.h"
#include "Log.h"

/*****************************************************************************/
Client::Client(IEvent &handler)
    : mEventHandler(handler)
    , mInitialized(false)
{

}
/*****************************************************************************/
void Client::Initialize()
{
    score.Reset();

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
    return dogDeck;
}
/*****************************************************************************/
Deck &Client::GetHandleDeck()
{
    return handleDeck;
}
/*****************************************************************************/
Deck &Client::GetMyDeck()
{
    return mPlayer.GetDeck();
}
/*****************************************************************************/
Identity &Client::GetMyIdentity()
{
    return mPlayer.GetIdentity();
}
/*****************************************************************************/
bool Client::TestHandle()
{
    mPlayer.GetDeck().AnalyzeTrumps(stats);
    if ((handleDeck.HasFool() == true) && (stats.trumps > handleDeck.size()))
    {
        return false;
    }
    else
    {
        return true;
    }
}
/*****************************************************************************/
Game &Client::GetGameInfo()
{
    return info;
}
/*****************************************************************************/
Score &Client::GetScore()
{
    return score;
}
/*****************************************************************************/
Place Client::GetPlace()
{
    return mPlayer.GetPlace();
}
/*****************************************************************************/
void Client::SetMyIdentity(const Identity &ident)
{
    mPlayer.SetIdentity(ident);
}
/*****************************************************************************/
void Client::SetDiscard(Deck &discard)
{
    // Add the dog to the payer's deck
    mPlayer.GetDeck() += dogDeck;

    // remove cards from the client's deck
    for (int i = 0; i < discard.size(); i++)
    {
        Card *c = discard.at(i);
        if (mPlayer.GetDeck().removeAll(c) != 1)
        {
            TLogError("Wrong number of cards");
        }
    }

    std::string message = "Dog: " + dogDeck.GetCardList();
    TLogInfo(message);
    TLogInfo("Player: " + mPlayer.GetDeck().GetCardList());
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
        cont = PASS;
    }
    else if (total >= 36  && total <= 50)
    {
        cont = TAKE;
    }
    else if (total >= 51  && total <= 65)
    {
        cont = GUARD;
    }
    else if (total >= 66  && total <= 75)
    {
        cont = GUARD_WITHOUT;
    }
    else
    {
        cont = GUARD_AGAINST;
    }
    return cont;
}
/*****************************************************************************/
void Client::UpdateStatistics()
{
    stats.Reset();
    mPlayer.GetDeck().AnalyzeTrumps(stats);
    mPlayer.GetDeck().AnalyzeSuits(stats);
}
/*****************************************************************************/
Deck Client::BuildDogDeck()
{
    int i;
    Card *c, *cdeck;
    Deck discard = dogDeck;

    TLogInfo("Auto discard before: " + discard.GetCardList());

    bool ok = false;
    i = 0;

    // We're looking for trumps or kings in the deck and we replace
    // them by other valid cards
    while (ok == false)
    {
        c = discard.at(i);
        if ((c->GetSuit() == Card::TRUMPS) ||
                ((c->GetSuit() != Card::TRUMPS) && (c->GetValue() == 14)))
        {
            // looking for valid card
            int k = mPlayer.GetDeck().size();
            for (int j = 0; j < k; j++)
            {
                cdeck = mPlayer.GetDeck().at(j);
                if ((cdeck->GetSuit() != Card::TRUMPS) && (cdeck->GetValue() < 14))
                {
                    // Swap cards
                    mPlayer.GetDeck().removeAll(cdeck);
                    mPlayer.GetDeck().append(c);
                    discard.removeAll(c);
                    discard.append(cdeck);
                    break;
                }
            }
            i = 0;
        }
        else
        {
            i++;
        }

        if (i == 6)
        {
            ok = true;
        }
    }

    TLogInfo("Auto discard after: " + discard.GetCardList());

    return discard;
}
/*****************************************************************************/
Card *Client::Play()
{
    Card *c = NULL;

    for (int i = 0; i < mPlayer.GetDeck().size(); i++)
    {
        c = mPlayer.GetDeck().at(i);
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
void Client::ConnectToHost(const std::string &hostName, std::uint16_t port)
{
    if (!mTcpClient.IsValid())
    {
        // Create a socket before connection
        mTcpClient.Create();
    }

    if (mTcpClient.Connect(hostName, port) == false)
    {
        TLogError("Client cannot connect to server.");
    }
}
/*****************************************************************************/
void Client::Close()
{
    mTcpClient.Close();
}
/*****************************************************************************/
void Client::EntryPoint(void *pthis)
{
    Client * pt = (Client*)pthis;
    pt->Run();
}
/*****************************************************************************/
void Client::Run()
{
    std::string buffer;
    while(true)
    {
        if (mTcpClient.IsValid())
        {
            if (mTcpClient.Recv(buffer) > 0)
            {
                ByteArray data(buffer);

                std::vector<Protocol::PacketInfo> packets = Protocol::DecodePacket(data);

                // Execute all packets
                for (std::uint16_t i = 0U; i< packets.size(); i++)
                {
                    Protocol::PacketInfo inf = packets[i];

                    ByteArray subArray = data.SubArray(inf.offset, inf.size);
                    DoAction(subArray);
                }
            }
        }
        else
        {
             std::this_thread::sleep_for(std::chrono::seconds(1U));
             // FIXME: replace by an event, wait for connection!
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
            break;
        }

        case Protocol::SERVER_REQUEST_IDENTITY:
        {
            std::uint8_t place;
            std::uint32_t myUuid;
            std::uint8_t nbPlayers;
            std::uint8_t mode;

            in >> place;
            in >> myUuid;
            in >> nbPlayers;
            in >> mode;

            mPlayer.SetPlace((Place)place);
            mPlayer.SetUuid(myUuid);
            info.Initialize(nbPlayers);
            info.gameMode = (Game::Mode)mode;
            SendIdentity();
            mEventHandler.AssignedPlace();
            break;
        }

        case Protocol::SERVER_PLAYERS_LIST:
        {
            std::uint8_t nombre;
            std::map<Place, Identity> players;

            in >> nombre;
            for (int i = 0; i < nombre; i++)
            {
                Identity ident;
                std::uint8_t place;

                in >> place;
                in >> ident;
                players[(Place)place] = ident;
            }
            mEventHandler.PlayersList(players);
            break;
        }

        case Protocol::SERVER_SEND_CARDS:
        {
            in >> mPlayer.GetDeck();
            score.Reset();
            UpdateStatistics();
            mEventHandler.ReceiveCards();
            break;
        }

        case Protocol::SERVER_REQUEST_BID:
        {
            std::uint8_t c;
            std::uint8_t p;

            in >> c; // Most important contract announced before
            in >> p;
            mEventHandler.SelectPlayer((Place)p);
            if (p == mPlayer.GetPlace())
            {
                // The request bid is for us! We must declare something
                mEventHandler.RequestBid((Contract)c);
            }
            break;
        }

        case Protocol::SERVER_SHOW_PLAYER_BID:
        {
            std::uint8_t c, p, slam;
            in >> p;
            in >> c;
            in >> slam;
            mEventHandler.ShowBid((Place)p, (slam == 1 ? true : false), (Contract)c);
            break;
        }

        case Protocol::SERVER_SHOW_DOG:
        {
            in >> dogDeck;
            dogDeck.Sort();
            info.sequence = Game::SHOW_DOG;
            mEventHandler.ShowDog();
            break;
        }

        case Protocol::SERVER_BUILD_DISCARD:
        {
            info.sequence = Game::BUILD_DOG;
            mEventHandler.BuildDiscard();
            break;
        }

        case Protocol::SERVER_START_DEAL:
        {
            std::uint8_t preneur;
            std::uint8_t contrat;
            Game::Shuffle sh;

            in >> preneur;
            in >> contrat;
            in >> sh;
            info.NewDeal();
            info.taker = (Place)preneur;
            info.contract = (Contract)contrat;
            currentTrick.clear();
            info.sequence = Game::SYNC_START;
            mEventHandler.StartDeal((Place)preneur, (Contract)contrat, sh);
            break;
        }

        case Protocol::SERVER_SHOW_HANDLE:
        {
            std::uint8_t p;
            in >> p;
            in >> handleDeck;
            if (GetGameInfo().taker == (Place)p)
            {
                handleDeck.SetOwner(ATTACK);
            }
            else
            {
                handleDeck.SetOwner(DEFENSE);
            }
            info.sequence = Game::SHOW_HANDLE;
            mEventHandler.ShowHandle();
            break;
        }

        case Protocol::SERVER_SHOW_CARD:
        {
            std::string name;
            std::uint8_t player;

            in >> player;
            in >> name;

            info.Next();
            currentTrick.append(TarotDeck::GetCard(name));
            info.sequence = Game::SYNC_CARD;
            mEventHandler.ShowCard((Place)player, name);
            break;
        }

        case Protocol::SERVER_PLAY_CARD:
        {
            std::uint8_t p;
            in >> p;

            mEventHandler.SelectPlayer((Place)p);
            if (p == mPlayer.GetPlace())
            {
                // Our turn to play a card
                info.sequence = Game::PLAY_TRICK;
                mEventHandler.PlayCard();
            }
            break;
        }

        case Protocol::SERVER_DEAL_AGAIN:
        {
            mEventHandler.DealAgain();
            break;
        }

        case Protocol::SERVER_END_OF_TRICK:
        {
            std::uint8_t winner;
            in >> winner;
            info.Next();
            info.sequence = Game::SYNC_TRICK;
            mEventHandler.WaitTrick((Place)winner);
            break;
        }

        case Protocol::SERVER_END_OF_DEAL:
        {
            in >> score;
            info.sequence = Game::SYNC_READY;
            mEventHandler.EndOfDeal();
            break;
        }

        case Protocol::SERVER_END_OF_GAME:
        {
            mEventHandler.EndOfGame();
            break;
        }

        default:
            std::string msg = mPlayer.GetIdentity().name + ": Unknown packet received.";
            TLogInfo(msg);
            ret = false;
            break;
    }
    return ret;
}
/*****************************************************************************/
void Client::SendIdentity()
{
    ByteArray packet = Protocol::BuildReplyIdentity(mPlayer.GetIdentity(), mPlayer.GetUuid());
    mTcpClient.Send(packet.ToSring());
}
/*****************************************************************************/
void Client::SendChatMessage(const std::string &message)
{
    ByteArray packet = Protocol::BuildClientChatMessage(message, mPlayer.GetUuid());
    mTcpClient.Send(packet.ToSring());
}
/*****************************************************************************/
void Client::SendReady()
{
    ByteArray packet = Protocol::BuildClientReady(mPlayer.GetUuid());
    mTcpClient.Send(packet.ToSring());
}
/*****************************************************************************/
void Client::SendError()
{
    ByteArray packet = Protocol::BuildClientError(mPlayer.GetUuid());
    mTcpClient.Send(packet.ToSring());
}
/*****************************************************************************/
void Client::SendBid(Contract c, bool slam)
{
    ByteArray packet = Protocol::BuildClientBid(c, slam, mPlayer.GetUuid());
    mTcpClient.Send(packet.ToSring());
}
/*****************************************************************************/
void Client::SendDiscard(const Deck &discard)
{
    ByteArray packet = Protocol::BuildClientDiscard(discard, mPlayer.GetUuid());
    mTcpClient.Send(packet.ToSring());
}
/*****************************************************************************/
void Client::SendHandle()
{
    ByteArray packet = Protocol::BuildClientHandle(handleDeck, mPlayer.GetUuid());
    mTcpClient.Send(packet.ToSring());
}
/*****************************************************************************/
void Client::SendCard(Card *c)
{
    if (c != NULL)
    {
        ByteArray packet = Protocol::BuildClientCard(c->GetName(), mPlayer.GetUuid());
        mTcpClient.Send(packet.ToSring());
    }
    else
    {
        SendError();
    }
}
/*****************************************************************************/
void Client::SendSyncDog()
{
    info.sequence = Game::IDLE;

    ByteArray packet = Protocol::BuildClientSyncDog(mPlayer.GetUuid());
    mTcpClient.Send(packet.ToSring());
}
/*****************************************************************************/
void Client::SendSyncStart()
{
    info.sequence = Game::IDLE;

    ByteArray packet = Protocol::BuildClientSyncStart(mPlayer.GetUuid());
    mTcpClient.Send(packet.ToSring());
}
/*****************************************************************************/
void Client::SendSyncCard()
{
    info.sequence = Game::IDLE;

    ByteArray packet = Protocol::BuildClientSyncCard(mPlayer.GetUuid());
    mTcpClient.Send(packet.ToSring());
}
/*****************************************************************************/
void Client::SendSyncBid()
{
    info.sequence = Game::IDLE;

    ByteArray packet = Protocol::BuildClientSyncBid(mPlayer.GetUuid());
    mTcpClient.Send(packet.ToSring());
}
/*****************************************************************************/
void Client::SendSyncTrick()
{
    info.sequence = Game::IDLE;
    currentTrick.clear();

    ByteArray packet = Protocol::BuildClientSyncTrick(mPlayer.GetUuid());
    mTcpClient.Send(packet.ToSring());
}
/*****************************************************************************/
void Client::SendSyncHandle()
{
    ByteArray packet = Protocol::BuildClientSyncHandle(mPlayer.GetUuid());
    mTcpClient.Send(packet.ToSring());
}

//=============================================================================
// End of file Client.cpp
//=============================================================================
