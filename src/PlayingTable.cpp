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
#include "PlayingTable.h"
#include "Network.h"
#include "System.h"
#include "Protocol.h"

// Ask steps translation into integer
struct Ack
{
    std::string step; // Textual value
    Engine::Sequence sequence; // Engine sequence
};

static const Ack gAckList[] = {
    {"AckJoinTable", Engine::WAIT_FOR_PLAYERS},
    {"Ready", Engine::WAIT_FOR_READY },
    {"NewDeal", Engine::WAIT_FOR_CARDS },
    {"ShowBid", Engine::WAIT_FOR_SHOW_BID },
    {"AllPassed", Engine::WAIT_FOR_ALL_PASSED },
    {"ShowDog", Engine::WAIT_FOR_SHOW_DOG },
    {"StartDeal", Engine::WAIT_FOR_START_DEAL },
    {"ShowHandle", Engine::WAIT_FOR_SHOW_HANDLE },
    {"Card", Engine::WAIT_FOR_SHOW_CARD },
    {"EndOfTrick", Engine::WAIT_FOR_END_OF_TRICK },
    {"EndOfDeal", Engine::WAIT_FOR_END_OF_DEAL }
};

static const std::uint32_t gAckListSize = sizeof(gAckList) / sizeof(gAckList[0]);

Engine::Sequence FindSequence(const std::string &step)
{
    Engine::Sequence seq = Engine::BAD_STEP;
    for (std::uint32_t i = 0U; i < gAckListSize; i++)
    {
        if (gAckList[i].step == step)
        {
            seq = gAckList[i].sequence;
            break;
        }
    }
    return seq;
}


/*****************************************************************************/
PlayingTable::PlayingTable()
    : mFull(false)
    , mAdmin(Protocol::INVALID_UID)
    , mName("Default")
    , mId(1U)
    , mAdminMode(false)
{

}
/*****************************************************************************/
bool PlayingTable::AckFromAllPlayers()
{
    bool ack = false;
    std::uint8_t counter = 0U;

    for (std::uint8_t i = 0U; i < mEngine.GetNbPlayers(); i++)
    {
        if (mPlayers[i].ack)
        {
            counter++;
        }
    }

    if (counter == mEngine.GetNbPlayers())
    {
        ack = true;
        ResetAck();
    }

    return ack;
}
/*****************************************************************************/
void PlayingTable::ResetAck()
{
    for (std::uint8_t i = 0U; i < mEngine.GetNbPlayers(); i++)
    {
        mPlayers[i].ack = false;
    }
}
/*****************************************************************************/
Place PlayingTable::GetPlayerPlace(std::uint32_t uuid)
{
    Place p = Place::NOWHERE;

    for (std::uint8_t i = 0U; i < mEngine.GetNbPlayers(); i++)
    {
        if (mPlayers[i].uuid == uuid)
        {
            p = i;
        }
    }
    return p;
}
/*****************************************************************************/
std::uint32_t PlayingTable::GetPlayerUuid(Place p)
{
    return mPlayers[p.Value()].uuid;
}
/*****************************************************************************/
void PlayingTable::SendToAllPlayers(std::vector<Reply> &out, JsonObject &obj)
{
    std::vector<std::uint32_t> list;
    for (std::uint32_t i = 0U; i < mEngine.GetNbPlayers(); i++)
    {
        if (!mPlayers[i].IsFree())
        {
            list.push_back(mPlayers[i].uuid);
        }
    }
    out.push_back(Reply(list, obj));
}
/*****************************************************************************/
bool PlayingTable::Sync(Engine::Sequence sequence, std::uint32_t uuid)
{
    for (std::uint32_t i = 0U; i < mEngine.GetNbPlayers(); i++)
    {
        if (mPlayers[i].uuid == uuid)
        {
            if (mEngine.GetSequence() == sequence)
            {
                mPlayers[i].ack = true;
            }
        }
    }
    return AckFromAllPlayers();
}
/*****************************************************************************/
void PlayingTable::Initialize()
{
    mEngine.Initialize();
}
/*****************************************************************************/
void PlayingTable::SetupGame(const Tarot::Game &game)
{
    mGame = game;
}
/*****************************************************************************/
void PlayingTable::SetAdminMode(bool enable)
{
    mAdminMode = enable;
}
/*****************************************************************************/
void PlayingTable::CreateTable(std::uint8_t nbPlayers)
{
    mEngine.CreateTable(nbPlayers);

    // close all the clients
    for (std::uint32_t i = 0U; i < 5U; i++)
    {
        mPlayers[i].Clear();
    }

    mFull = false;
    mAdmin = Protocol::INVALID_UID;
}
/*****************************************************************************/
Place PlayingTable::AddPlayer(std::uint32_t uuid, std::uint8_t &nbPlayers)
{
    Place assigned;
    nbPlayers = mEngine.GetNbPlayers();

    if (mEngine.GetSequence() == Engine::WAIT_FOR_PLAYERS)
    {
        // Check if player is not already connected
        if (GetPlayerPlace(uuid) == Place::NOWHERE)
        {
            // Look for free Place and assign the uuid to this player
            for (std::uint32_t i = 0U; i < mEngine.GetNbPlayers(); i++)
            {
                if (mPlayers[i].uuid == Protocol::INVALID_UID)
                {
                    assigned = i;
                    break;
                }
            }

            std::uint8_t place = assigned.Value();
            if (place < Place::NOWHERE)
            {
                mPlayers[place].uuid = uuid;
                // If it is the first player, then it is an admin
                if (mAdmin == Protocol::INVALID_UID)
                {
                    mAdmin = uuid;
                }
            }
            else
            {
                TLogError("Internal memory problem");
            }
        }
    }
    return assigned;
}
/*****************************************************************************/
bool PlayingTable::RemovePlayer(std::uint32_t kicked_player)
{
    bool removeAllPlayers = false;

    // Check if the uuid exists
    Place place = GetPlayerPlace(kicked_player);
    if (place != Place::NOWHERE)
    {
        // If we are waiting for players, continue to wait
        if (mEngine.GetSequence() == Engine::WAIT_FOR_PLAYERS)
        {
            // Update the admin
            if (kicked_player == mAdmin)
            {
                std::uint32_t newAdmin = Protocol::INVALID_UID;
                for (std::uint32_t i = 0U; i < mEngine.GetNbPlayers(); i++)
                {
                    // Choose another admin
                    std::uint32_t uuid = GetPlayerUuid(Place(i));
                    if (uuid != kicked_player)
                    {
                        newAdmin = uuid;
                        break;
                    }
                }
                mAdmin = newAdmin;
            }

            // Actually remove it
            for (std::uint32_t i = 0U; i < mEngine.GetNbPlayers(); i++)
            {
                if (mPlayers[i].uuid == kicked_player)
                {
                    mPlayers[i].Clear();
                }
            }
        }
        else
        {
            // If we are in a game, finish it and kick all players
            removeAllPlayers = true;
            mEngine.CreateTable(mEngine.GetNbPlayers()); // recreate the table (== reset it)
        }
    }

    return removeAllPlayers;
}
/*****************************************************************************/
void PlayingTable::ExecuteRequest(const std::string &cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const JsonValue &json, std::vector<Reply> &out)
{
    (void) dest_uuid;

    if (cmd == "Error")
    {
        TLogError("Client has sent an error code");
    }
    else if (cmd == "Ack")
    {
        // Check if the uuid exists
        if (GetPlayerPlace(src_uuid) != Place::NOWHERE)
        {
            std::string step = json.FindValue("step").GetString();
            Engine::Sequence seq = FindSequence(step);

            if (seq == Engine::BAD_STEP)
            {
                TLogError("Bad acknowledge sequence");
            }
            else
            {
                TLogNetwork("Received sync() for step: " + step);

                // Returns true if all the players have send their sync signal
                if (Sync(seq, src_uuid))
                {
                    TLogNetwork("All players have sync() for step: " + step);
                    switch (seq) {
                    case Engine::WAIT_FOR_PLAYERS:
                    {
                        if (!mAdminMode)
                        {
                            // Automatic start of the game. Otherwise the Admin is in charge of starting manually the game
                            NewGame(out);
                        }
                        break;
                    }
                    case Engine::WAIT_FOR_READY:
                    case Engine::WAIT_FOR_ALL_PASSED:
                    {
                        NewDeal(out);
                        break;
                    }
                    case Engine::WAIT_FOR_CARDS:
                    case Engine::WAIT_FOR_SHOW_BID:
                    {
                        BidSequence(out);
                        break;
                    }
                    case Engine::WAIT_FOR_SHOW_DOG:
                    {
                        // When all the players have seen the dog, ask to the taker to build a discard
                        mEngine.DiscardSequence();
                        JsonObject obj;

                        obj.AddValue("cmd", "BuildDiscard");
                        out.push_back(Reply(GetPlayerUuid(mEngine.GetBid().taker), obj));
                        break;
                    }
                    case Engine::WAIT_FOR_START_DEAL:
                    case Engine::WAIT_FOR_SHOW_HANDLE:
                    case Engine::WAIT_FOR_SHOW_CARD:
                    case Engine::WAIT_FOR_END_OF_TRICK:
                    {
                        GameSequence(out);
                        break;
                    }
                    case Engine::WAIT_FOR_END_OF_DEAL:
                    {
                        EndOfDeal(out);
                        break;
                    }
                    default:
                        break;
                    }
                }
            }
        }
    }
    else if (cmd == "RequestNewGame")
    {
        if (src_uuid == mAdmin)
        {
            mGame.Set(json.FindValue("mode").GetString());
            mGame.deals.clear();
            JsonArray deals = json.FindValue("deals").GetArray();

            for (std::uint32_t i = 0U; i < deals.Size(); i++)
            {
                JsonObject obj = deals.GetEntry(i).GetObj();

                Tarot::Distribution dist;
                FromJson(dist, obj);
                mGame.deals.push_back(dist);
            }

            NewGame(out);
        }
    }
    else if (cmd == "ReplyBid")
    {
        Contract c(json.FindValue("contract").GetString());
        bool slam = json.FindValue("slam").GetBool();

        // Check if the uuid exists
        Place p = GetPlayerPlace(src_uuid);
        if (p != Place::NOWHERE)
        {
            // Check if this is the right player
            if (p == mEngine.GetCurrentPlayer())
            {
                // Check if we are in the good sequence
                if (mEngine.GetSequence() == Engine::WAIT_FOR_BID)
                {
                    Contract cont = mEngine.SetBid((Contract)c, slam, p);

                    // Broadcast player's bid, and wait for all acknowlegements
                    JsonObject obj;

                    obj.AddValue("cmd", "ShowBid");
                    obj.AddValue("place", p.ToString());
                    obj.AddValue("contract", cont.ToString());
                    obj.AddValue("slam", slam);

                    SendToAllPlayers(out, obj);
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
    }
    else if (cmd == "Discard")
    {
        Deck discard(json.FindValue("discard").GetString());

        // Check sequence
        if (mEngine.GetSequence() == Engine::WAIT_FOR_DISCARD)
        {
            // Check if right player
            if (mEngine.GetBid().taker == GetPlayerPlace(src_uuid))
            {
                if (mEngine.SetDiscard(discard))
                {
                    // Then start the deal
                    StartDeal(out);
                }
                else
                {
                    TLogError("Not a valid discard" + discard.ToString());
                }
            }
        }
    }
    else if (cmd == "Handle")
    {
        Deck handle(json.FindValue("handle").GetString());

        // Check sequence
        if (mEngine.GetSequence() == Engine::WAIT_FOR_HANDLE)
        {
            // Check if right player
            Place p =  GetPlayerPlace(src_uuid);
            if (mEngine.GetCurrentPlayer() == p)
            {
                if (mEngine.SetHandle(handle, p))
                {
                    // Handle is valid, show it to all players
                    JsonObject obj;

                    obj.AddValue("cmd", "ShowHandle");
                    obj.AddValue("place", p.ToString());
                    obj.AddValue("handle", handle.ToString());

                    SendToAllPlayers(out, obj);
                }
                else
                {
                    // Invalid or no handle, continue game (player has to play a card)
                    GameSequence(out);
                }
            }
        }
    }
    else if (cmd == "Card")
    {
        Card c(json.FindValue("card").GetString());

        // Check if the card name exists
        if (c.IsValid())
        {
            // Check sequence
            if (mEngine.GetSequence() == Engine::WAIT_FOR_PLAYED_CARD)
            {
                Place p =  GetPlayerPlace(src_uuid);
                // Check if right player
                if (mEngine.GetCurrentPlayer() == p)
                {
                    if (mEngine.SetCard(c, p))
                    {
                        // Broadcast played card, and wait for all acknowlegements
                        JsonObject obj;

                        obj.AddValue("cmd", "ShowCard");
                        obj.AddValue("place", p.ToString());
                        obj.AddValue("card", c.ToString());

                        SendToAllPlayers(out, obj);
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
            TLogError("Bad card name!");
        }
    }
    else
    {
        TLogError("Unknown packet received");
    }
}
/*****************************************************************************/
void PlayingTable::EndOfDeal(std::vector<Reply> &out)
{
    bool continueGame = mScore.AddPoints(mEngine.GetCurrentGamePoints(), mEngine.GetBid(), mEngine.GetNbPlayers());

    if (continueGame)
    {
        NewDeal(out);
    }
    else
    {
        // No more deal, send a end of game
        mEngine.NewGame();
        JsonObject obj;

        obj.AddValue("cmd", "EndOfGame");
        obj.AddValue("winner", mScore.GetWinner().ToString());

        SendToAllPlayers(out, obj);
    }
}
/*****************************************************************************/
void PlayingTable::NewGame(std::vector<Reply> &out)
{
    JsonObject obj;
    mScore.NewGame(mGame.deals.size());
    mEngine.NewGame();
    ResetAck();

    // Inform players about the game type
    obj.AddValue("cmd", "NewGame");
    obj.AddValue("mode", mGame.Get());

    SendToAllPlayers(out, obj);
}
/*****************************************************************************/
void PlayingTable::NewDeal(std::vector<Reply> &out)
{
    if (mScore.GetCurrentCounter() >= mGame.deals.size())
    {
        // Consider a rollover, start a new game
        mScore.NewGame(mGame.deals.size());
    }

    mEngine.NewDeal(mGame.deals[mScore.GetCurrentCounter()]);
    // Send the cards to all the players
    for (std::uint32_t i = 0U; i < mEngine.GetNbPlayers(); i++)
    {
        JsonObject obj;
        Place place(i);

        Deck deck = mEngine.GetDeck(place);
        obj.AddValue("cmd", "NewDeal");
        obj.AddValue("cards", deck.ToString());

        out.push_back(Reply(GetPlayerUuid(place), obj));
    }
}
/*****************************************************************************/
void PlayingTable::StartDeal(std::vector<Reply> &out)
{
    Place first = mEngine.StartDeal();
    Tarot::Bid bid = mEngine.GetBid();
    JsonObject obj;

    obj.AddValue("cmd", "StartDeal");
    obj.AddValue("first_player", first.ToString());
    obj.AddValue("taker", bid.taker.ToString());
    obj.AddValue("contract", bid.contract.ToString());
    obj.AddValue("slam", bid.slam);

    ToJson(mGame.deals[mScore.GetCurrentCounter()], obj);

    SendToAllPlayers(out, obj);
}
/*****************************************************************************/
void PlayingTable::BidSequence(std::vector<Reply> &out)
{
    // Launch/continue the bid sequence
    mEngine.BidSequence();

    Engine::Sequence seq = mEngine.GetSequence();
    switch (seq)
    {
    case Engine::WAIT_FOR_BID:
    {
        JsonObject obj;

        obj.AddValue("cmd", "RequestBid");
        obj.AddValue("place", mEngine.GetCurrentPlayer().ToString());
        obj.AddValue("contract", mEngine.GetBid().contract.ToString());
        SendToAllPlayers(out, obj);
        break;
    }

    case Engine::WAIT_FOR_ALL_PASSED:
    {
        JsonObject obj;

        obj.AddValue("cmd", "AllPassed");
        SendToAllPlayers(out, obj);
        break;
    }
    case Engine::WAIT_FOR_START_DEAL:
    {
        StartDeal(out);
        break;
    }

    case Engine::WAIT_FOR_SHOW_DOG:
    {
        JsonObject obj;

        obj.AddValue("cmd", "ShowDog");
        obj.AddValue("dog", mEngine.GetDog().ToString());
        SendToAllPlayers(out, obj);
        break;
    }

    default:
        TLogError("Bad game sequence for bid");
        break;
    }
}
/*****************************************************************************/
void PlayingTable::GameSequence(std::vector<Reply> &out)
{
    mEngine.GameSequence();

    if (mEngine.IsLastTrick())
    {
        JsonObject deal;
        JsonObject obj;
        Points points = mEngine.GetCurrentGamePoints();

        mEngine.EndOfDeal(deal);

        obj.AddValue("cmd", "EndOfDeal");
        obj.AddValue("deal", deal);
        obj.AddValue("points", points.pointsAttack);
        obj.AddValue("oudlers", points.oudlers);
        obj.AddValue("little_bonus", points.littleEndianOwner.Value());
        obj.AddValue("handle_bonus", points.handlePoints);
        obj.AddValue("slam_bonus", points.slamDone);

        SendToAllPlayers(out, obj);
    }
    else
    {
        Engine::Sequence seq = mEngine.GetSequence();

        Place p = mEngine.GetCurrentPlayer();

        switch (seq)
        {
        case Engine::WAIT_FOR_END_OF_TRICK:
        {
            JsonObject obj;

            obj.AddValue("cmd", "EndOfTrick");
            obj.AddValue("place", p.ToString());
            SendToAllPlayers(out, obj);
            break;
        }
        case Engine::WAIT_FOR_PLAYED_CARD:
        {
            JsonObject obj;

            obj.AddValue("cmd", "PlayCard");
            obj.AddValue("place", p.ToString());
            SendToAllPlayers(out, obj);
            break;
        }
        case Engine::WAIT_FOR_HANDLE:
        {
            JsonObject obj;

            obj.AddValue("cmd", "AskForHandle");
            out.push_back(Reply(GetPlayerUuid(p), obj));
        }
        break;

        default:
            TLogError("Bad sequence, game engine state problem");
            break;
        }
    }
}

//=============================================================================
// End of file Server.cpp
//=============================================================================
