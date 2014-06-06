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
    : engine(*this)
    , mInitialized(false)
{

}
/*****************************************************************************/
void Controller::RegisterListener(Observer<Controller::Signal> &observer)
{
    mSubject.Attach(observer);
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
void Controller::StartDeal()
{
    SendPacket(Protocol::BuildStartDeal(engine.GetBid(),
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

    case Protocol::ADMIN_CREATE_GAME:
    {
        std::uint8_t gameMode;
        std::uint8_t nbPlayers;
        Game::Shuffle shuffle;

        in >> gameMode;
        in >> nbPlayers;
        in >> shuffle;
        engine.CreateGame((Tarot::GameMode)gameMode, shuffle, nbPlayers);
        break;
    }

    case Protocol::ADMIN_ADD_PLAYER:
    {
        std::uint32_t newplayer_uuid;
        in >> newplayer_uuid;
        // Look for free Place and assign the uuid to this player
        Place assigned = engine.AddPlayer(newplayer_uuid);
        if (assigned.Value() != Place::NOWHERE)
        {
            // New player connected, ask for its identity
            SendPacket(Protocol::BuildRequestIdentity(
                           assigned,
                           engine.GetNbPlayers(),
                           engine.GetGameInfo().gameMode,
                           newplayer_uuid));
        }
        else
        {
            // Server is full, send an error message
            SendPacket(Protocol::BuildErrorServerFull(uuid));
        }
        break;
    }

    case Protocol::CLIENT_INFOS:
    {
        Identity ident;
        in >> ident;

        // Check if the uuid exists
        if (engine.GetPlayerPlace(uuid) != NOWHERE)
        {
            bool full = engine.SetIdentity(uuid, ident);

            std::string message = "The player " + ident.name + " has joined the game.";
            SendPacket(Protocol::BuildServerChatMessage(message));

            if (full)
            {
                SendPacket(Protocol::BuildPlayersList(engine.GetPlayersList()));
                SignalGameFull();
            }
        }
        break;
    }

    case Protocol::CLIENT_MESSAGE:
    {
        std::string message;
        in >> message;

        std::string name = engine.GetPlayer(engine.GetPlayerPlace(uuid)).GetIdentity().name;
        message = name + "> " + message;

        SendPacket(Protocol::BuildServerChatMessage(message));
        break;
    }

    case Protocol::ADMIN_NEW_DEAL:
    {
        engine.NewDeal();
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
            if (engine.SetHandle(handle, p))
            {
                // Handle is valid, show it to all players
                SendPacket(Protocol::BuildShowHandle(handle, p));
            }
        }
        else
        {
            TLogError("Cannot get player place from UUID");
        }
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

    case Protocol::CLIENT_SYNC_SHOW_CARD:
    {
        engine.SyncCard();
        break;
    }

    case Protocol::CLIENT_SYNC_READY:
    {
        // Check if the uuid exists
        if (engine.GetPlayerPlace(uuid) != Place::NOWHERE)
        {
            if (engine.Sync(TarotEngine::WAIT_FOR_READY, uuid))
            {
                NewDeal();
            }
        }
        break;
    }

    case::Protocol::CLIENT_SYNC_CARDS:
    {
        // Check if the uuid exists
        if (engine.GetPlayerPlace(uuid) != Place::NOWHERE)
        {
            if (engine.Sync(TarotEngine::WAIT_FOR_CARDS, uuid))
            {
                // All the players have received their cards, launch the bid sequence
                BidSequence();
            }
        }
    }

    case Protocol::CLIENT_BID:
    {
        std::uint8_t c;
        std::uint8_t slam;

        in >> c;
        in >> slam;

        // Check if the uuid exists
        Place p = engine.GetPlayerPlace(uuid);
        if (p != Place::NOWHERE)
        {
            // Check if this is the right player
            if (p == engine.GetCurrentPlayer())
            {
                // Check if we are in the good sequence
                if (engine.GetSequence() == TarotEngine::WAIT_FOR_BID)
                {
                    Contract cont = engine.SetBid((Contract)c, (slam == 1U ? true : false), p);
                    // Broadcast player's bid, and wait for all acknowlegements
                    SendPacket(Protocol::BuildShowBid(cont, (slam == 1U ? true : false), p));
                }
                else
                {
                    TLogError("Wrong sequence");
                }
            }
            else
            {
                TLogError("Wrong player to bid");
            }
        }
        else
        {
            TLogError("Cannot get player place from UUID");
        }
        break;
    }

    case Protocol::CLIENT_SYNC_SHOW_BID:
    {
        // Check if the uuid exists
        if (engine.GetPlayerPlace(uuid) != Place::NOWHERE)
        {
            if (engine.Sync(TarotEngine::WAIT_FOR_SHOW_BID, uuid))
            {
                // Continue/finish the bid sequence
                BidSequence();
            }
        }
        break;
    }

    case Protocol::CLIENT_SYNC_SHOW_DOG:
    {
        // Check if the uuid exists
        if (engine.GetPlayerPlace(uuid) != Place::NOWHERE)
        {
            if (engine.Sync(TarotEngine::WAIT_FOR_SHOW_DOG, uuid))
            {
                // When all the players have seen the dog, ask to the taker to build a discard
                Player *player = engine.GetPlayer(engine.GetBid().place);
                std::uint32_t id = player.GetUuid();
                SendPacket(Protocol::BuildDiscardRequest(id));
            }
        }
        break;
    }

    case Protocol::CLIENT_DISCARD:
    {
        Deck discard;
        in >> discard;

        // Check if the uuid exists
        Place p = engine.GetPlayerPlace(uuid);
        if (p != Place::NOWHERE)
        {
            // Check sequence
            if (engine.GetSequence() == TarotEngine::WAIT_FOR_DISCARD)
            {
                // Check if right player
                if (engine.GetBid().place == p)
                {
                    engine.SetDiscard(discard);
                    // Then start the deal
                    SendPacket(Protocol::BuildStartDeal(engine.GetBid(), engine.GetShuffle()));
                }
            }
        }
        break;
    }

    case Protocol::CLIENT_SYNC_START:
    {
        // Check if the uuid exists
        if (engine.GetPlayerPlace(uuid) != Place::NOWHERE)
        {
            if (engine.Sync(TarotEngine::WAIT_FOR_START_DEAL, uuid))
            {
                engine.GameSequence();
            }
        }
        break;
    }


    default:
        TLogError("Unknown packet received");
        break;
    }

    return ret;
}
/*****************************************************************************/
void Controller::NewDeal()
{
    engine.NewDeal();
    // Send the cards to all the players
    for (std::uint32_t i = 0U; i < engine.GetNbPlayers(); i++)
    {
        Player *player = engine.GetPlayer(i);
        SendPacket(Protocol::BuildSendCards(player));
    }
}
/*****************************************************************************/
void Controller::BidSequence()
{
    // Launch/continue the bid sequence
    TarotEngine::BidResult res = engine.BidSequence();

    switch (res)
    {
    case TarotEngine::NEXT_PLAYER:
        SendPacket(Protocol::BuildBidRequest(engine.GetBid().contract, engine.GetCurrentPlayer()));
        break;

    case TarotEngine::ALL_PASSED:
        NewDeal();
        break;

    case TarotEngine::START_DEAL:
        SendPacket(Protocol::BuildStartDeal(engine.GetBid(), engine.GetShuffle()));
        break;

    case TarotEngine::SHOW_DOG:
        SendPacket(Protocol::BuildShowDog(engine.GetDog()));
        break;

    default:
        TLogError("Dad enum TarotEngine::BidResult value");
        break;
    }
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
