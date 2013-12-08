/*=============================================================================
 * TarotClub - Server.cpp
 *=============================================================================
 * Host a Tarot game and manage network clients
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
#include <string>
#include "Log.h"
#include "Server.h"
#include "ByteStreamReader.h"

/*****************************************************************************/
Server::Server()
{
    // Register ourself as an observer of Tarot Engine events
    engine.RegisterListener(*this);
}
/*****************************************************************************/
void Server::Update(const TarotEngine::SignalInfo &info)
{
    switch (info.sig)
    {
    case TarotEngine::SIG_REQUEST_BID:
        slotSendRequestBid(info.c, info.p);
        break;
    case TarotEngine::SIG_DEAL_AGAIN:
        slotSendDealAgain();
        break;
    case TarotEngine::SIG_PLAY_CARD:
        slotSendPlayCard(info.p);
        break;
    case TarotEngine::SIG_END_OF_TRICK:
        slotSendWaitTrick(info.p);
        break;
    case TarotEngine::SIG_END_OF_DEAL:
        slotSendEndOfDeal();
        break;
    case TarotEngine::SIG_SEND_CARDS:
        slotSendCards();
        break;
    case TarotEngine::SIG_SHOW_DOG:
        slotSendShowDog();
        break;
    case TarotEngine::SIG_START_DEAL:
        slotSendStartDeal();
        break;
        default:
        break;
    }
}
/*****************************************************************************/
void Server::NewServerGame(Game::Mode mode)
{
    // Init everything
    engine.NewGame(mode);
}
/*****************************************************************************/
void Server::Start()
{
    mThread = std::thread(Server::EntryPoint, this);
}
/*****************************************************************************/
void Server::ExecuteRequest(const ByteArray &packet)
{
    mQueue.Push(packet);
}
/*****************************************************************************/
void Server::Run()
{
    ByteArray data;
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1U));
        mQueue.WaitAndPop(data);

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
/*****************************************************************************/
void Server::EntryPoint(void *pthis)
{
    Server * pt = (Server*)pthis;
    pt->Run();
}
/*****************************************************************************/
bool Server::DoAction(const ByteArray &data)
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
        case Protocol::ADMIN_NEW_SERVER_GAME:
        {
            std::uint8_t gameMode;
            std::uint8_t nbPlayers;

            in >> gameMode;
            in >> nbPlayers;
            NewServerGame((Game::Mode)gameMode);
            break;
        }

        case Protocol::ADMIN_NEW_PLAYER:
        {
            // Look for free Place
            Place assigned = engine.GetFreePlayer();
            if (assigned != NOWHERE)
            {
                // Assign the uuid to this player
                engine.GetPlayer(assigned).SetUuid(uuid);
                SendPacket(Protocol::BuildRequestIdentity(
                               assigned,
                               engine.GetGameInfo().numberOfPlayers,
                               engine.GetGameInfo().gameMode,
                               uuid));
            }
            else
            {
                SendPacket(Protocol::BuildErrorServerFull(uuid));
            }
            break;
        }

        case Protocol::CLIENT_MESSAGE:
        {
            std::string message;
            in >> message;
            SendPacket(Protocol::BuildServerChatMessage(message));
            break;
        }

        case Protocol::CLIENT_INFOS:
        {
            Identity ident;
            in >> ident;

            ident.avatar = ":/images/avatars/" + ident.avatar;
            Player *player = engine.GetPlayer(uuid);
            if (player)
            {
                player->SetIdentity(ident);
            }
            else
            {
                TLogError("Fatal error, Uuid not found!");
            }
            std::string message = "The player " + ident.name + " has joined the game.";
            SendPacket(Protocol::BuildServerChatMessage(message));

            if (engine.GetNumberOfCurrentPlayers() == engine.GetGameInfo().numberOfPlayers)
            {
                SendPlayersList();
                engine.NewDeal();
            }
            break;
        }

        case Protocol::CLIENT_BID:
        {
            std::uint8_t c;
            std::uint8_t slam;

            in >> c;
            in >> slam;

            Place p = engine.GetPlayerPlace(uuid);

            if (p != NOWHERE)
            {
                // FIXME: add protection like client origin, current sequence...

                Contract cont = engine.SetBid((Contract)c, (slam == 1 ? true : false), p);
                // Broadcast player's bid, and wait for all acknowlegements
                SendShowBid(cont, (slam == 1 ? true : false), p);
            }
            break;
        }

        case Protocol::CLIENT_SYNC_DOG:
        {
            if (engine.SyncDog() == true)
            {
                std::uint32_t id = engine.GetPlayer(engine.GetGameInfo().taker)->GetUuid();
                SendPacket(Protocol::BuildDiscardOrder(id));
            }
            break;
        }

        case Protocol::CLIENT_DISCARD:
        {
            Deck discard;
            in >> discard;

            // FIXME: add protectiontest if the sender is the right player
            engine.SetDiscard(discard);
            break;
        }

        case Protocol::CLIENT_HANDLE:
        {
            // FIXME: add protections: handle validity, game sequence ...
            Deck handle;
            in >> handle;
            engine.SetHandle(handle, p);
            SendShowHandle(handle, p);
            break;
        }

        case Protocol::CLIENT_SYNC_START:
        {
            engine.SyncStart();
            break;
        }

        case Protocol::CLIENT_SYNC_BID:
        {
            engine.SyncBid();
            break;
        }

        case Protocol::CLIENT_SYNC_TRICK:
        {
            engine.SyncTrick();
            break;
        }

        case Protocol::CLIENT_SYNC_HANDLE:
        {
            engine.SyncHandle();
            break;
        }

        case Protocol::CLIENT_CARD:
        {
            std::string card;
            Card *c;

            // FIXME: tester la validité de la carte (ID + présence dans le jeu du joueur)
            // REFUSER SI MAUVAISE SEQUENCE EN COURS
            // si erreur : logguer et prévenir quelqu'un ??

            in >> card;
            c = TarotDeck::GetCard(card);
            if (c != NULL)
            {
                engine.SetCard(c, p);

                // Broadcast played card, and wait for all acknowlegements
                SendShowCard(c);
            }
            else
            {
                TLogError("Bad card name!");
            }
            break;
        }

        case Protocol::CLIENT_SYNC_CARD:
        {
            engine.SyncCard();
            break;
        }

        case Protocol::CLIENT_READY:
        {
            engine.SyncReady();
            break;
        }

        default:
            TLogError("Unkown packet received");
            ret = false;
            break;
    }

    return ret;
}
/*****************************************************************************/
void Server::SendShowBid(Contract c, bool slam, Place p)
{
    ByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (std::uint8_t)p   // current player bid
        << (std::uint8_t)c;  // contract to show
    if (slam)
    {
        out << (std::uint8_t)1;
    }
    else
    {
        out << (std::uint8_t)0;
    }
    packet = Protocol::BuildHeader(packet, Protocol::SERVER_SHOW_PLAYER_BID);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::SendDisconnect()
{
    ByteArray packet;
    packet = Protocol::BuildHeader(packet, Protocol::SERVER_DISCONNECT);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::SendPlayersList()
{
    ByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (std::uint8_t)engine.GetNumberOfCurrentPlayers();
    for (int i = 0; i < engine.GetGameInfo().numberOfPlayers; i++)
    {
        if (engine.GetPlayer((Place)i).IsFree() == false)
        {
            Identity ident = engine.GetPlayer((Place)i).GetIdentity();
            out << (std::uint8_t)i;
            out << ident;
        }
    }
    packet = Protocol::BuildHeader(packet, Protocol::SERVER_PLAYERS_LIST);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::SendShowCard(Card *c)
{
    ByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (std::uint8_t)c->GetId()
        << (std::uint8_t)engine.GetGameInfo().currentPlayer;
    packet = Protocol::BuildHeader(packet, Protocol::SERVER_SHOW_CARD);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::SendShowHandle(Deck &handle, Place p)
{
    ByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (std::uint8_t)p;
    out << handle;
    packet = Protocol::BuildHeader(packet, Protocol::SERVER_SHOW_HANDLE);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendCards()
{
    for (int i = 0; i < engine.GetGameInfo().numberOfPlayers; i++)
    {
        ByteArray packet;
        QDataStream out(&packet, QIODevice::WriteOnly);
        out << engine.GetPlayer((Place)i).GetDeck();
        packet = Protocol::BuildHeader(packet, Protocol::SERVER_SEND_CARDS);
        SendPacket((Place)i, packet);
    }
}
/*****************************************************************************/
void Server::slotSendEndOfDeal()
{
    ByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << engine.GetScore();
    packet = Protocol::BuildHeader(packet, Protocol::SERVER_END_OF_DEAL);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendWaitTrick(Place winner)
{
    ByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (std::uint8_t)winner;
    packet = Protocol::BuildHeader(packet, Protocol::SERVER_END_OF_TRICK);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendStartDeal()
{
    ByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (std::uint8_t)engine.GetGameInfo().taker
        << (std::uint8_t)engine.GetGameInfo().contract;
    packet = Protocol::BuildHeader(packet, Protocol::SERVER_START_DEAL);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendPlayCard(Place p)
{
    ByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (std::uint8_t)p; // this player has to play a card
    packet = Protocol::BuildHeader(packet, Protocol::SERVER_PLAY_CARD);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendRequestBid(Contract c, Place p)
{
    ByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (std::uint8_t)c; // previous bid
    out << (std::uint8_t)p; // player to declare something
    packet = Protocol::BuildHeader(packet, Protocol::SERVER_REQUEST_BID);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendShowDog()
{
    ByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << engine.GetDeal().GetDog();
    packet = Protocol::BuildHeader(packet, Protocol::SERVER_SHOW_DOG);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendDealAgain()
{
    ByteArray packet;
    packet = Protocol::BuildHeader(packet, Protocol::SERVER_DEAL_AGAIN);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::Broadcast(ByteArray &block)
{
    SendPacket(Protocol::ALL_PLAYERS, block);
}
/*****************************************************************************/
void Server::SendPacket(const ByteArray &block)
{
    Signal sig;

    sig.data = block;
    mSubject.Notify(sig);
}

//=============================================================================
// End of file Server.cpp
//=============================================================================
