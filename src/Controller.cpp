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
#include "Controller.h"
#include "ByteStreamReader.h"

/*****************************************************************************/
Controller::Controller()
    : mInitialized(false)
    , engine(*this)
{

}
/*****************************************************************************/
void Controller::RegisterListener(Observer<Controller::Signal> &observer)
{
    mSubject.Attach(observer);
}
/*****************************************************************************/
void Controller::NewServerGame(Game::Mode mode)
{
    // Init everything
    engine.NewGame(mode);
}
/*****************************************************************************/
void Controller::RequestBid(Contract c, Place p)
{
    SendPacket(Protocol::BuildBidRequest(c, p));
}
/*****************************************************************************/
void Controller::DealAgain()
{
    SendPacket(Protocol::BuildDealAgain());
}
/*****************************************************************************/
void Controller::PlayCard(Place p)
{
    SendPacket(Protocol::BuildPlayCard(p));
}
/*****************************************************************************/
void Controller::EndOfTrick(Place p)
{
    SendPacket(Protocol::BuildEndOfTrick(p));
}
/*****************************************************************************/
void Controller::EndOfDeal()
{
    SendPacket(Protocol::BuildEndOfDeal(engine.GetScore()));
}
/*****************************************************************************/
void Controller::SendCards()
{
    for (std::uint32_t i = 0U; i < engine.GetGameInfo().numberOfPlayers; i++)
    {
        Player &player = engine.GetPlayer((Place)i);
        SendPacket(Protocol::BuildSendCards(player.GetUuid(), player.GetDeck()));
    }
}
/*****************************************************************************/
void Controller::ShowDog()
{
    SendPacket(Protocol::BuildShowDog(engine.GetDeal().GetDog()));
}
/*****************************************************************************/
void Controller::StartDeal()
{
    SendPacket(Protocol::BuildStartDeal(engine.GetGameInfo().taker,
                                        engine.GetGameInfo().contract,
                                        engine.GetShuffle()));
}
/*****************************************************************************/
void Controller::Start()
{
    engine.Initialize();
    if (!mInitialized)
    {
        mThread = std::thread(Controller::EntryPoint, this);
    }
}
/*****************************************************************************/
void Controller::Stop()
{
    mQueue.Push(Protocol::BuildQuitGame());
    mThread.join();
}
/*****************************************************************************/
void Controller::ExecuteRequest(const ByteArray &packet)
{
    mQueue.Push(packet);
}
/*****************************************************************************/
void Controller::EntryPoint(void *pthis)
{
    Controller * pt = (Controller*)pthis;
    pt->Run();
}
/*****************************************************************************/
void Controller::Run()
{
    ByteArray data;
    while(true)
    {
        mQueue.WaitAndPop(data);

        std::vector<Protocol::PacketInfo> packets = Protocol::DecodePacket(data);

        // Execute all packets
        for (std::uint16_t i = 0U; i< packets.size(); i++)
        {
            Protocol::PacketInfo inf = packets[i];

            ByteArray subArray = data.SubArray(inf.offset, inf.size);
            if (!DoAction(subArray))
            {
                // Quit thread
                return;
            }
        }
    }
}
/*****************************************************************************/
bool Controller::DoAction(const ByteArray &data)
{
    bool ret = true;
    ByteStreamReader in(data);

    // Jump over some header bytes
    in.Seek(3U);

    // Get the id of the sender
    std::uint32_t uuid;
    in >> uuid;

    // Get the command
    std::uint8_t cmd;
    in >> cmd;

    switch (cmd)
    {

    case Protocol::ADMIN_QUIT_GAME:
    {
        ret = false;
        break;
    }

    case Protocol::ADMIN_NEW_SERVER_GAME:
    {
        std::uint8_t gameMode;
        std::uint8_t nbPlayers;

        in >> gameMode;
        in >> nbPlayers;
        NewServerGame((Game::Mode)gameMode);
        break;
    }

    case Protocol::ADMIN_ADD_PLAYER:
    {
        std::uint32_t newplayer_uuid;
        in >> newplayer_uuid;
        // Look for free Place
        Place assigned = engine.GetFreePlayer();
        if (assigned.Value() != Place::NOWHERE)
        {
            // Assign the uuid to this player
            engine.GetPlayer(assigned).SetUuid(newplayer_uuid);
            SendPacket(Protocol::BuildRequestIdentity(
                           assigned,
                           engine.GetGameInfo().numberOfPlayers,
                           engine.GetGameInfo().gameMode,
                           newplayer_uuid));
        }
        else
        {
            SendPacket(Protocol::BuildErrorServerFull(uuid));
        }
        break;
    }

    case Protocol::ADMIN_START_DEAL:
    {
        engine.NewDeal();
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

        bool full = engine.SetIdentity(uuid, ident);

        std::string message = "The player " + ident.name + " has joined the game.";
        SendPacket(Protocol::BuildServerChatMessage(message));

        if (full)
        {
            std::map<Place, Identity> players;

            for (std::uint32_t i = 0U; i < engine.GetGameInfo().numberOfPlayers; i++)
            {
                players[(Place)i] = engine.GetPlayer((Place)i).GetIdentity();
            }
            SendPacket(Protocol::BuildPlayersList(players));
            SignalGameFull();
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

        if (p.Value() != Place::NOWHERE)
        {
            // FIXME: add protection like client origin, current sequence...

            Contract cont = engine.SetBid((Contract)c, (slam == 1 ? true : false), p);
            // Broadcast player's bid, and wait for all acknowlegements
            SendPacket(Protocol::BuildShowBid(cont, (slam == 1 ? true : false), p));
        }
        else
        {
            TLogError("Cannot get player place from UUID");
        }
        break;
    }

    case Protocol::CLIENT_SYNC_DOG:
    {
        if (engine.SyncDog() == true)
        {
            Player &player = engine.GetPlayer(engine.GetGameInfo().taker);
            std::uint32_t id = player.GetUuid();
            SendPacket(Protocol::BuildDiscardRequest(id));
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

        Place p = engine.GetPlayerPlace(uuid);

        if (p.Value() != Place::NOWHERE)
        {
            engine.SetHandle(handle, p);
            SendPacket(Protocol::BuildShowHandle(handle, p));

        }
        else
        {
            TLogError("Cannot get player place from UUID");
        }
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
            Place p = engine.GetPlayerPlace(uuid);

            if (p.Value() != Place::NOWHERE)
            {
                engine.SetCard(c, p);
                // Broadcast played card, and wait for all acknowlegements
                SendPacket(Protocol::BuildShowCard(c, p));
            }
            else
            {
                TLogError("Cannot get player place from UUID");
            }
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
        TLogError("Unknown packet received");
        break;
    }

    return ret;
}
/*****************************************************************************/
void Controller::SendPacket(const ByteArray &block)
{
    Signal sig;

    sig.type = SIG_SEND_DATA;
    sig.data = block;
    mSubject.Notify(sig);
}
/*****************************************************************************/
void Controller::SignalGameFull()
{
    Signal sig;

    sig.type = SIG_GAME_FULL;
    mSubject.Notify(sig);
}


//=============================================================================
// End of file Server.cpp
//=============================================================================
