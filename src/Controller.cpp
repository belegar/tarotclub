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
#include "System.h"

/*****************************************************************************/
Controller::Controller(IData &handler)
    : mDataHandler(handler)
    , mFull(false)
    , mAdmin(Protocol::INVALID_UID)
    , mName("Default")
    , mId(1U)
    , mAdminMode(false)
{

}
/*****************************************************************************/
void Controller::Initialize()
{
    mEngine.Initialize();
}
/*****************************************************************************/
void Controller::ExecuteRequest(const ByteArray &packet)
{
    mQueue.Push(packet); // Add packet to the queue
    Protocol::GetInstance().Execute(this); // Actually decode the packet
}
/*****************************************************************************/
void Controller::SetupGame(const Tarot::Game &game)
{
    mGame = game;
}
/*****************************************************************************/
void Controller::SetAdminMode(bool enable)
{
    mAdminMode = enable;
}
/*****************************************************************************/
bool Controller::DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data)
{
    (void) dest_uuid;
    bool ret = true;
    ByteStreamReader in(data);

    std::stringstream dbg;
    dbg << "Server controller command received: 0x" << std::hex << (int)cmd;
    TLogNetwork(dbg.str());

    switch (cmd)
    {
    case Protocol::CLIENT_ERROR:
    {
        TLogError("Client has sent an error code");
        break;
    }

    case Protocol::LOBBY_CREATE_TABLE:
    {
        if (src_uuid == Protocol::LOBBY_UID)
        {
            std::uint8_t nbPlayers;

            in >> nbPlayers;

            mEngine.CreateTable(nbPlayers);
            mFull = false;
            mAdmin = Protocol::INVALID_UID;
        }
        break;
    }

    case Protocol::LOBBY_ADD_PLAYER:
    {
        std::uint32_t uuid;
        Identity ident;
        in >> uuid;
        in >> ident;

        // Check if player is not already connected
        if (mEngine.GetPlayerPlace(uuid) == Place::NOWHERE)
        {
            // Look for free Place and assign the uuid to this player
            Place assigned = mEngine.AddPlayer(uuid);
            if (assigned.Value() != Place::NOWHERE)
            {
                // New player connected, send table information
                Send(Protocol::TableJoinReply(
										   true,
                                           assigned,
                                           mEngine.GetNbPlayers(),
                                           uuid));

                // Warn upper layers
                Send(Protocol::TableAcceptedPlayer(uuid, mId));

                // If it is the first player, then it is an admin
                if (mAdmin == Protocol::INVALID_UID)
                {
                    mAdmin = uuid;
                }

                mPlayers[mEngine.GetPlayerPlace(uuid).Value()] = ident;
                mFull = mEngine.ValidatePlayer(uuid);
                std::string message = "The player " + ident.nickname + " has joined the game.";
                Send(Protocol::TableChatMessage(message));

                // Update player list
                Send(Protocol::TablePlayersList(CreatePlayerList()));
                if (mFull)
                {
                    if (mAdminMode)
                    {
                        // Warn table admin that the game is full, so it can start
                        Send(Protocol::AdminGameFull(true, mAdmin));
                    }
                    else
                    {
                        // Automatic start of the game
                        NewGame();
                    }
                }
            }
            else
            {
                // Server is full, send an error message
                Send(Protocol::TableFullMessage(uuid));
            }
        }
        else
        {
            // Player is already connected, send error
            // New player connected, send table information
            Send(Protocol::TableJoinReply(
                                       false,
                                       Place::NOWHERE,
                                       0U,
                                       uuid));

        }
        break;
    }

    case Protocol::LOBBY_REMOVE_PLAYER:
    {
        std::uint32_t kicked_player;
        in >> kicked_player;
        // Check if the uuid exists
        Place place = mEngine.GetPlayerPlace(kicked_player);
        if (place != Place::NOWHERE)
        {
            // Warn all the player
            std::string message = "The player " + mPlayers[place.Value()].nickname + " has quit the game.";
            Send(Protocol::TableChatMessage(message));

            // Remove this player from the engine
            mEngine.RemovePlayer(kicked_player);
            Send(Protocol::TableRemovedPlayer(kicked_player, mId));
            // Update player list
            Send(Protocol::TablePlayersList(CreatePlayerList()));
            // Update the admin
            if (kicked_player == mAdmin)
            {
                // Choose another admin
                std::uint32_t newAdmin = Protocol::INVALID_UID;
                for (std::uint32_t i = 0U; i < mEngine.GetNbPlayers(); i++)
                {
                    Player *player = mEngine.GetPlayer(i);
                    if (player != NULL)
                    {
                        newAdmin = player->GetUuid();
                    }
                }
                mAdmin = newAdmin;
            }

            // If we are in a game, finish it and kick all players
            if (mEngine.GetSequence() != TarotEngine::WAIT_FOR_PLAYERS)
            {
                // Remove remaining players
                for (std::uint32_t i = 0U; i < mEngine.GetNbPlayers(); i++)
                {
                    Player *player = mEngine.GetPlayer(Place(i));
                    if (player != NULL)
                    {
                        Send(Protocol::TableRemovedPlayer(player->GetUuid(), mId));
                    }
                }
                mEngine.CreateTable(mEngine.GetNbPlayers());
                mFull = false;
                mAdmin = Protocol::INVALID_UID;
            }
        }
        break;
    }

    case Protocol::CLIENT_TABLE_MESSAGE:
    {
        std::string message;
        in >> message;

        // Check if the uuid exists
        Place place = mEngine.GetPlayerPlace(src_uuid);
        if (place != Place::NOWHERE)
        {
            message = mPlayers[place.Value()].nickname + "> " + message;
            Send(Protocol::TableChatMessage(message));
        }
        break;
    }

    case Protocol::ADMIN_NEW_GAME:
    {
        if (src_uuid == mAdmin)
        {
            in >> mGame;
            NewGame();
        }
        break;
    }

    case Protocol::CLIENT_SYNC_NEW_GAME:
    {
        // Check if the uuid exists
        if (mEngine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
        {
            if (mEngine.Sync(TarotEngine::WAIT_FOR_READY, src_uuid))
            {
                NewDeal();
            }
        }
        break;
    }

    case Protocol::CLIENT_SYNC_NEW_DEAL:
    {
        // Check if the uuid exists
        if (mEngine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
        {
            if (mEngine.Sync(TarotEngine::WAIT_FOR_CARDS, src_uuid))
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
        Place p = mEngine.GetPlayerPlace(src_uuid);
        if (p != Place::NOWHERE)
        {
            // Check if this is the right player
            if (p == mEngine.GetCurrentPlayer())
            {
                // Check if we are in the good sequence
                if (mEngine.GetSequence() == TarotEngine::WAIT_FOR_BID)
                {
                    Contract cont = mEngine.SetBid((Contract)c, (slam == 1U ? true : false), p);
                    // Broadcast player's bid, and wait for all acknowlegements
                    Send(Protocol::TableShowBid(cont, (slam == 1U ? true : false), p));
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
        if (mEngine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
        {
            if (mEngine.Sync(TarotEngine::WAIT_FOR_SHOW_BID, src_uuid))
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
        if (mEngine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
        {
            if (mEngine.Sync(TarotEngine::WAIT_FOR_ALL_PASSED, src_uuid))
            {
                NewDeal();
            }
        }
        break;
    }

    case Protocol::CLIENT_SYNC_SHOW_DOG:
    {
        // Check if the uuid exists
        if (mEngine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
        {
            if (mEngine.Sync(TarotEngine::WAIT_FOR_SHOW_DOG, src_uuid))
            {
                // When all the players have seen the dog, ask to the taker to build a discard
                Player *player = mEngine.GetPlayer(mEngine.GetBid().taker);
                if (player != NULL)
                {
                    mEngine.DiscardSequence();
                    std::uint32_t id = player->GetUuid();
                    Send(Protocol::TableAskForDiscard(id));
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
        Place p = mEngine.GetPlayerPlace(src_uuid);
        if (p != Place::NOWHERE)
        {
            // Check sequence
            if (mEngine.GetSequence() == TarotEngine::WAIT_FOR_DISCARD)
            {
                // Check if right player
                if (mEngine.GetBid().taker == p)
                {
                    if (mEngine.SetDiscard(discard))
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
        if (mEngine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
        {
            if (mEngine.Sync(TarotEngine::WAIT_FOR_START_DEAL, src_uuid))
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

        Place p = mEngine.GetPlayerPlace(src_uuid);

        // Check if the uuid exists
        if (p.Value() != Place::NOWHERE)
        {
            // Check sequence
            if (mEngine.GetSequence() == TarotEngine::WAIT_FOR_HANDLE)
            {
                // Check if right player
                if (mEngine.GetCurrentPlayer() == p)
                {
                    if (mEngine.SetHandle(handle, p))
                    {
                        // Handle is valid, show it to all players
                        Send(Protocol::TableShowHandle(handle, p));
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
        if (mEngine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
        {
            if (mEngine.Sync(TarotEngine::WAIT_FOR_SHOW_HANDLE, src_uuid))
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
            Place p = mEngine.GetPlayerPlace(src_uuid);

            // Check if the uuid exists
            if (p.Value() != Place::NOWHERE)
            {
                // Check sequence
                if (mEngine.GetSequence() == TarotEngine::WAIT_FOR_PLAYED_CARD)
                {
                    // Check if right player
                    if (mEngine.GetCurrentPlayer() == p)
                    {
                        if (mEngine.SetCard(c, p))
                        {
                            // Broadcast played card, and wait for all acknowlegements
                            Send(Protocol::TableShowCard(c, p));
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
        if (mEngine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
        {
            if (mEngine.Sync(TarotEngine::WAIT_FOR_SHOW_CARD, src_uuid))
            {
                GameSequence();
            }
        }
        break;
    }

    case Protocol::CLIENT_SYNC_TRICK:
    {
        // Check if the uuid exists
        if (mEngine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
        {
            if (mEngine.Sync(TarotEngine::WAIT_FOR_END_OF_TRICK, src_uuid))
            {
                GameSequence();
            }
        }
        break;
    }

    case Protocol::CLIENT_SYNC_END_OF_DEAL:
    {
        // Check if the uuid exists
        if (mEngine.GetPlayerPlace(src_uuid) != Place::NOWHERE)
        {
            if (mEngine.Sync(TarotEngine::WAIT_FOR_END_OF_DEAL, src_uuid))
            {
                EndOfDeal();
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
void Controller::EndOfDeal()
{
    bool continueGame = mScore.AddPoints(mEngine.GetCurrentGamePoints(), mEngine.GetBid(), mEngine.GetNbPlayers());

    if (continueGame)
    {
        NewDeal();
    }
    else
    {
        mEngine.StopGame();
        Send(Protocol::TableEndOfGame(mScore.GetWinner()));
    }
}
/*****************************************************************************/
std::map<Place, Identity> Controller::CreatePlayerList()
{
    std::map<Place, Identity> list;

    for (std::uint8_t i = 0U; i < mEngine.GetNbPlayers(); i++)
    {
        list[Place(i)] = mPlayers[i];
    }

    return list;
}
/*****************************************************************************/
void Controller::NewGame()
{
    mScore.NewGame(mGame.deals.size());
    mEngine.NewGame();
    Send(Protocol::TableNewGame(mGame));
}
/*****************************************************************************/
void Controller::NewDeal()
{
    if (mScore.GetCurrentCounter() < mGame.deals.size())
    {
        mEngine.NewDeal(mGame.deals[mScore.GetCurrentCounter()]);
        // Send the cards to all the players
        for (std::uint32_t i = 0U; i < mEngine.GetNbPlayers(); i++)
        {
            Player *player = mEngine.GetPlayer(Place(i));
            if (player != NULL)
            {
                Send(Protocol::TableNewDeal(player));
            }
            else
            {
                TLogError("Cannot get player deck");
            }
        }
    }
    else
    {
        TLogError("Deals or game counter size problem");
    }
}
/*****************************************************************************/
void Controller::StartDeal()
{
    Place first = mEngine.StartDeal();
    Send(Protocol::TableStartDeal(first, mEngine.GetBid(), mGame.deals[mScore.GetCurrentCounter()]));
}
/*****************************************************************************/
void Controller::BidSequence()
{
    // Launch/continue the bid sequence
    mEngine.BidSequence();

    TarotEngine::Sequence seq = mEngine.GetSequence();
    switch (seq)
    {
        case TarotEngine::WAIT_FOR_BID:
            Send(Protocol::TableBidRequest(mEngine.GetBid().contract, mEngine.GetCurrentPlayer()));
            break;

        case TarotEngine::WAIT_FOR_ALL_PASSED:
            Send(Protocol::TableAllPassed());
            break;

        case TarotEngine::WAIT_FOR_START_DEAL:
            StartDeal();
            break;

        case TarotEngine::WAIT_FOR_SHOW_DOG:
            Send(Protocol::TableShowDog(mEngine.GetDog()));
            break;

        default:
            TLogError("Bad game sequence for bid");
            break;
    }
}
/*****************************************************************************/
void Controller::GameSequence()
{
    mEngine.GameSequence();

    if (mEngine.IsLastTrick())
    {
        std::stringstream ss;
        ss << System::ProjectName() << "_" << mName << "_" << mId << "_";
        std::string json = mEngine.EndOfDeal(mPlayers, ss.str());
        Send(Protocol::TableEndOfDeal(mEngine.GetCurrentGamePoints(), json));
    }
    else
    {
        TarotEngine::Sequence seq = mEngine.GetSequence();

        Place p = mEngine.GetCurrentPlayer();

        switch (seq)
        {
            case TarotEngine::WAIT_FOR_END_OF_TRICK:
                Send(Protocol::TableEndOfTrick(p));
                break;

            case TarotEngine::WAIT_FOR_PLAYED_CARD:
                Send(Protocol::TablePlayCard(p));
                break;

            case TarotEngine::WAIT_FOR_HANDLE:
            {
                Player *player = mEngine.GetPlayer(p);
                if (player != NULL)
                {
                    Send(Protocol::TableAskForHandle(player->GetUuid()));
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
void Controller::Send(const ByteArray &block)
{
    std::uint8_t cmd = block.Get(Protocol::COMMAND_OFFSET);
    std::stringstream dbg;
    dbg << "Server controller sending packet: 0x" << std::hex << (int)cmd;
    TLogNetwork(dbg.str());

    mDataHandler.SendData(block, mId);
}


//=============================================================================
// End of file Server.cpp
//=============================================================================
