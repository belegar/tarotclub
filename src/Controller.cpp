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
Controller::Controller(IEvent &handler)
    : mEventHandler(handler)
    , mFull(false)
    , mName("Default")
    , mId(1U)
{

}
/*****************************************************************************/
void Controller::Initialize()
{
    engine.Initialize();
}
/*****************************************************************************/
void Controller::ExecuteRequest(const ByteArray &packet)
{
    mQueue.Push(packet); // Add packet to the queue
    Protocol::GetInstance().Execute(this); // Actually decode the packet
}
/*****************************************************************************/
bool Controller::DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data)
{
    (void) dest_uuid;
    bool ret = true;
    ByteStreamReader in(data);

    switch (cmd)
    {
        case Protocol::LOBBY_QUIT_GAME:
        {
            if (src_uuid == Protocol::SYSTEM_UID)
            {
                ret = false;
            }
            break;
        }

        case Protocol::CLIENT_ERROR:
        {
            TLogError("Client has sent an error code");
            break;
        }

        case Protocol::LOBBY_CREATE_TABLE:
        {
            if (src_uuid == Protocol::SYSTEM_UID)
            {
                std::uint8_t nbPlayers;

                in >> nbPlayers;

                engine.CreateTable(nbPlayers);
                mFull = false;
                mAdmins.clear();
            }
            break;
        }

        case Protocol::LOBBY_ADD_PLAYER:
        {
            std::uint32_t uuid;
            Identity ident;
            in >> uuid;
            in >> ident;
            // Look for free Place and assign the uuid to this player
            Place assigned = engine.AddPlayer(uuid);
            if (assigned.Value() != Place::NOWHERE)
            {
                // New player connected, send table information
                mEventHandler.SendData(Protocol::TableJoinReply(
                                           assigned,
                                           engine.GetNbPlayers(),
                                           uuid));

                // Warn upper layers
                mEventHandler.AcceptPlayer(uuid, mId);

                // If it is the first player, then it is an admin
                if (mAdmins.empty())
                {
                    mAdmins.push_back(uuid);
                }
            }
            else
            {
                // Server is full, send an error message
                mEventHandler.SendData(Protocol::TableFullMessage(uuid));
            }
            break;
        }

        case Protocol::LOBBY_REMOVE_PLAYER:
        {
            std::uint32_t kicked_player;
            in >> kicked_player;
            // Check if the uuid exists
             Place place = engine.GetPlayerPlace(kicked_player);
            if (place != Place::NOWHERE)
            {
                // Warn all the player
                std::string message = "The player " + engine.GetIdentity(place).name + " has quit the game.";
                mEventHandler.SendData(Protocol::TableChatMessage(message));

                // FIXME: put the game in "pause" mode
                // Ask for a new player to continue
            }
            break;
        }

        case Protocol::CLIENT_MESSAGE:
        {
            std::string message;
            in >> message;

            // Check if the uuid exists
            Place place = engine.GetPlayerPlace(src_uuid);
            if (place != Place::NOWHERE)
            {
                message = engine.GetIdentity(place).name + "> " + message;
                mEventHandler.SendData(Protocol::TableChatMessage(message));
            }
            break;
        }

        case Protocol::ADMIN_NEW_GAME:
        {
            if (IsAdmin(src_uuid))
            {
                std::uint8_t gameMode;
                Tarot::Shuffle shuffle;

                in >> gameMode;
                in >> shuffle;

                engine.NewGame((Tarot::GameMode)gameMode, shuffle);
                mEventHandler.SendData(Protocol::TableNewGame((Tarot::GameMode)gameMode, shuffle));
            }
            break;
        }

        case Protocol::CLIENT_SYNC_NEW_GAME:
        {
            // Check if the uuid exists
            if (engine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
            {
                if (engine.Sync(TarotEngine::WAIT_FOR_READY, src_uuid))
                {
                    NewDeal();
                }
            }
            break;
        }

        case Protocol::CLIENT_SYNC_NEW_DEAL:
        {
            // Check if the uuid exists
            if (engine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
            {
                if (engine.Sync(TarotEngine::WAIT_FOR_CARDS, src_uuid))
                {
                    // All the players have received their cards, launch the bid sequence
                    BidSequence();
                }
            }
            break;
        }

        case Protocol::CLIENT_BID:
        {
            std::uint8_t c;
            std::uint8_t slam;

            in >> c;
            in >> slam;

            // Check if the uuid exists
            Place p = engine.GetPlayerPlace(src_uuid);
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
                        mEventHandler.SendData(Protocol::TableShowBid(cont, (slam == 1U ? true : false), p));
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
            if (engine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
            {
                if (engine.Sync(TarotEngine::WAIT_FOR_SHOW_BID, src_uuid))
                {
                    // Continue/finish the bid sequence
                    BidSequence();
                }
            }
            break;
        }

        case Protocol::CLIENT_SYNC_ALL_PASSED:
        {
            // Check if the uuid exists
            if (engine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
            {
                if (engine.Sync(TarotEngine::WAIT_FOR_ALL_PASSED, src_uuid))
                {
                    NewDeal();
                }
            }
            break;
        }

        case Protocol::CLIENT_SYNC_SHOW_DOG:
        {
            // Check if the uuid exists
            if (engine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
            {
                if (engine.Sync(TarotEngine::WAIT_FOR_SHOW_DOG, src_uuid))
                {
                    // When all the players have seen the dog, ask to the taker to build a discard
                    Player *player = engine.GetPlayer(engine.GetBid().taker);
                    if (player != NULL)
                    {
                        engine.DiscardSequence();
                        std::uint32_t id = player->GetUuid();
                        mEventHandler.SendData(Protocol::TableAskForDiscard(id));
                    }
                    else
                    {
                        TLogError("Cannot get player pointer!");
                    }
                }
            }
            break;
        }

        case Protocol::CLIENT_DISCARD:
        {
            Deck discard;
            in >> discard;

            // Check if the uuid exists
            Place p = engine.GetPlayerPlace(src_uuid);
            if (p != Place::NOWHERE)
            {
                // Check sequence
                if (engine.GetSequence() == TarotEngine::WAIT_FOR_DISCARD)
                {
                    // Check if right player
                    if (engine.GetBid().taker == p)
                    {
                        if (engine.SetDiscard(discard))
                        {
                            // Then start the deal
                            StartDeal();
                        }
                        else
                        {
                            TLogError("Not a valid discard" + discard.ToString());
                        }
                    }
                }
            }
            break;
        }

        case Protocol::CLIENT_SYNC_START:
        {
            // Check if the uuid exists
            if (engine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
            {
                if (engine.Sync(TarotEngine::WAIT_FOR_START_DEAL, src_uuid))
                {
                    GameSequence();
                }
            }
            break;
        }

        case Protocol::CLIENT_HANDLE:
        {
            Deck handle;
            in >> handle;

            Place p = engine.GetPlayerPlace(src_uuid);

            // Check if the uuid exists
            if (p.Value() != Place::NOWHERE)
            {
                // Check sequence
                if (engine.GetSequence() == TarotEngine::WAIT_FOR_HANDLE)
                {
                    // Check if right player
                    if (engine.GetCurrentPlayer() == p)
                    {
                        if (engine.SetHandle(handle, p))
                        {
                            // Handle is valid, show it to all players
                            mEventHandler.SendData(Protocol::TableShowHandle(handle, p));
                        }
                        else
                        {
                            // Invalid or no handle, continue game (player has to play a card)
                            GameSequence();
                        }
                    }
                }
            }
            else
            {
                TLogError("Cannot get player place from UUID");
            }
            break;
        }

        case Protocol::CLIENT_SYNC_HANDLE:
        {
            // Check if the uuid exists
            if (engine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
            {
                if (engine.Sync(TarotEngine::WAIT_FOR_SHOW_HANDLE, src_uuid))
                {
                    GameSequence();
                }
            }
            break;
        }

        case Protocol::CLIENT_CARD:
        {
            std::string cardName;

            in >> cardName;
            Card c(cardName);

            // Check if the card name exists
            if (c.IsValid())
            {
                Place p = engine.GetPlayerPlace(src_uuid);

                // Check if the uuid exists
                if (p.Value() != Place::NOWHERE)
                {
                    // Check sequence
                    if (engine.GetSequence() == TarotEngine::WAIT_FOR_PLAYED_CARD)
                    {
                        // Check if right player
                        if (engine.GetCurrentPlayer() == p)
                        {
                            if (engine.SetCard(c, p))
                            {
                                // Broadcast played card, and wait for all acknowlegements
                                mEventHandler.SendData(Protocol::TableShowCard(c, p));
                            }
                        }
                        else
                        {
                            TLogError("Wrong player");
                        }
                    }
                    else
                    {
                        TLogError("Bad sequence");
                    }
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
            // Check if the uuid exists
            if (engine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
            {
                if (engine.Sync(TarotEngine::WAIT_FOR_SHOW_CARD, src_uuid))
                {
                    GameSequence();
                }
            }
            break;
        }

        case Protocol::CLIENT_SYNC_TRICK:
        {
            // Check if the uuid exists
            if (engine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
            {
                if (engine.Sync(TarotEngine::WAIT_FOR_END_OF_TRICK, src_uuid))
                {
                    GameSequence();
                }
            }
            break;
        }

        case Protocol::CLIENT_SYNC_END_OF_DEAL:
        {
            // Check if the uuid exists
            if (engine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
            {
                if (engine.Sync(TarotEngine::WAIT_FOR_END_OF_DEAL, src_uuid))
                {
                    if (engine.NextDeal())
                    {
                        NewDeal();
                    }
                    else
                    {
                        mEventHandler.SendData(Protocol::TableEndOfGame(engine.GetWinner()));
                    }
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
ByteArray Controller::GetPacket()
{
    ByteArray data;
    if (!mQueue.TryPop(data))
    {
        TLogError("Work item called without any data in the queue!");
    }
    return data;
}
/*****************************************************************************/
void Controller::NewDeal()
{
    engine.NewDeal();
    // Send the cards to all the players
    for (std::uint32_t i = 0U; i < engine.GetNbPlayers(); i++)
    {
        Player *player = engine.GetPlayer(Place(i));
        if (player != NULL)
        {
            mEventHandler.SendData(Protocol::TableNewDeal(player));
        }
        else
        {
            TLogError("Cannot get player deck");
        }
    }
}
/*****************************************************************************/
void Controller::StartDeal()
{
    Place first = engine.StartDeal();
    mEventHandler.SendData(Protocol::TableStartDeal(first, engine.GetBid(), engine.GetShuffle()));
}
/*****************************************************************************/
void Controller::BidSequence()
{
    // Launch/continue the bid sequence
    engine.BidSequence();

    TarotEngine::Sequence seq = engine.GetSequence();
    switch (seq)
    {
        case TarotEngine::WAIT_FOR_BID:
            mEventHandler.SendData(Protocol::TableBidRequest(engine.GetBid().contract, engine.GetCurrentPlayer()));
            break;

        case TarotEngine::WAIT_FOR_ALL_PASSED:
            mEventHandler.SendData(Protocol::TableAllPassed());
            break;

        case TarotEngine::WAIT_FOR_START_DEAL:
            StartDeal();
            break;

        case TarotEngine::WAIT_FOR_SHOW_DOG:
            mEventHandler.SendData(Protocol::TableShowDog(engine.GetDog()));
            break;

        default:
            TLogError("Bad game sequence for bid");
            break;
    }
}
/*****************************************************************************/
void Controller::GameSequence()
{
    engine.GameSequence();

    if (engine.IsLastTrick())
    {
        engine.EndOfDeal();
        mEventHandler.SendData(Protocol::TableEndOfDeal(engine.GetScore()));
    }
    else
    {
        TarotEngine::Sequence seq = engine.GetSequence();

        Place p = engine.GetCurrentPlayer();

        switch (seq)
        {
            case TarotEngine::WAIT_FOR_END_OF_TRICK:
                mEventHandler.SendData(Protocol::TableEndOfTrick(p));
                break;

            case TarotEngine::WAIT_FOR_PLAYED_CARD:
                mEventHandler.SendData(Protocol::TablePlayCard(p));
                break;

            case TarotEngine::WAIT_FOR_HANDLE:
            {
                Player *player = engine.GetPlayer(p);
                if (player != NULL)
                {
                    mEventHandler.SendData(Protocol::TableAskForHandle(player->GetUuid()));
                }
                else
                {
                    TLogError("Big problem here...");
                }
            }
            break;

            default:
                TLogError("Bad sequence, game engine state problem");
                break;
        }
    }
}
/*****************************************************************************/
bool Controller::IsAdmin(std::uint32_t unique_id)
{
    bool ret = false;
    if (std::count(mAdmins.begin(), mAdmins.end(), unique_id) > 0)
    {
        ret = true;
    }
    return ret;
}


//=============================================================================
// End of file Server.cpp
//=============================================================================
