/*=============================================================================
 * TarotClub - JsonClient.cpp
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

#include "JsonClient.h"
#include "Identity.h"
#include "Log.h"
#include "JsonValue.h"
#include "JsonReader.h"

/*****************************************************************************/
bool ClientHelper::TestDiscard(const Deck &discard)
{
    return Player::TestDiscard(discard, mDog, mNbPlayers);
}
/*****************************************************************************/
Contract ClientHelper::CalculateBid()
{
    int total = 0;
    Contract cont;

    UpdateStatistics();

    // Set points according to the card values
    if (mStats.bigTrump == true)
    {
        total += 9;
    }
    if (mStats.fool == true)
    {
        total += 7;
    }
    if (mStats.littleTrump == true)
    {
        if (mStats.trumps == 5)
        {
            total += 5;
        }
        else if (mStats.trumps == 6 || mStats.trumps == 7)
        {
            total += 7;
        }
        else if (mStats.trumps > 7)
        {
            total += 8;
        }
    }

    // Each trump is 1 point
    // Each major trump is 1 more point
    total += mStats.trumps * 2;
    total += mStats.majorTrumps * 2;
    total += mStats.kings * 6;
    total += mStats.queens * 3;
    total += mStats.knights * 2;
    total += mStats.jacks;
    total += mStats.weddings;
    total += mStats.longSuits * 5;
    total += mStats.cuts * 5;
    total += mStats.singletons * 3;
    total += mStats.sequences * 4;

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
void ClientHelper::UpdateStatistics()
{
    mStats.Reset();
    AnalyzeTrumps(mStats);
    AnalyzeSuits(mStats);
}
/*****************************************************************************/
Card ClientHelper::Play()
{
    Card c;

    for (Deck::ConstIterator it = Begin(); it != End(); ++it)
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
bool ClientHelper::IsValid(const Card &c)
{
    return Player::CanPlayCard(c, mCurrentTrick);
}
/*****************************************************************************/
Deck ClientHelper::AutoDiscard()
{
    Deck discard = Player::AutoDiscard(mDog, mNbPlayers);
    Player::RemoveDuplicates(discard);
    return discard;
}
/*****************************************************************************/



/*****************************************************************************/
JsonClient::JsonClient(IEvent &handler)
    : mEventHandler(handler)
{

}
/*****************************************************************************/
bool JsonClient::DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data)
{
    bool ret = true;
    ByteStreamReader in(data);
    std::string command;
    std::uint32_t error = 0U;
    JsonObject arg;

    switch (cmd)
    {

    case Protocol::LOBBY_CHAT_MESSAGE:
    {
        std::string message;
        in >> message;
        arg.AddValue("target", dest_uuid);
        arg.AddValue("message", message);
        command = "ChatMessage";
        break;
    }

    case Protocol::LOBBY_DISCONNECT:
    {
        command = "KickedFromLobby";
        break;
    }

    case Protocol::LOBBY_REQUEST_LOGIN:
    {
        std::uint32_t myUuid;
        in >> myUuid;
        arg.AddValue("uuid", myUuid);
        command = "RequestLogin";
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

            JsonArray array;
            for (std::uint32_t i = 0U; i < tableSize; i++)
            {
                JsonObject object;
                std::string name;
                std::uint32_t id;

                in >> name;
                in >> id;
                object.AddValue("name", name);
                object.AddValue("id", id);
                array.AddValue(object);
            }
            arg.AddValue("tables", array);
            command = "EnteredLobby";
        }
        else
        {
            error = IEvent::ErrLobbyAccessRefused;
        }
        break;
    }

    case Protocol::LOBBY_PLAYERS_LIST:
    {
        std::uint8_t size;
        in >> size;

        JsonArray array;
        for (int i = 0; i < size; i++)
        {
            JsonObject object;
            Identity ident;
            std::uint32_t uuid;

            in >> uuid;
            in >> ident;

            object.AddValue("nickname", ident.nickname);
            object.AddValue("avatar", ident.avatar);
            object.AddValue("gender", ident.gender);
            object.AddValue("uuid", uuid);
            array.AddValue(object);
        }
        arg.AddValue("players", array);
        command = "LobbyPlayersList";
        break;
    }

    case Protocol::ADMIN_GAME_FULL:
    {
        bool full;
        in >> full;
        arg.AddValue("full", full);
        command = "AdminGameFull";
        break;
    }
    case Protocol::TABLE_ERROR_FULL:
    {
        error = IEvent::ErrTableFull;
        break;
    }

    case Protocol::LOBBY_JOIN_TABLE_REPLY:
    {
        bool status;
        std::uint32_t tableId;
        Place place;
        std::uint8_t nbPlayers;

        in >> tableId;
        in >> status;
        in >> place;
        in >> nbPlayers;
        if (status)
        {
            arg.AddValue("table", tableId);
            arg.AddValue("place", place.ToString());
            arg.AddValue("nb_players", nbPlayers);
            command = "TableJoinEvent";
        }
        else
        {
            error = IEvent::ErrTableAccessRefused;
        }
        break;
    }

    case Protocol::TABLE_QUIT_EVENT:
    {
        arg.AddValue("table", src_uuid);
        command = "TableQuitEvent";
        break;
    }

    case Protocol::TABLE_PLAYERS_LIST:
    {
        std::uint8_t size;
        in >> size;

        JsonArray array;
        for (int i = 0; i < size; i++)
        {
            JsonObject object;
            std::uint32_t uuid;
            Place place;

            in >> place;
            in >> uuid;
            object.AddValue("uuid", uuid);
            object.AddValue("place", place.ToString());
            array.AddValue(object);
        }
        arg.AddValue("players", array);
        command = "TablePlayersList";
        break;
    }

    case Protocol::TABLE_NEW_GAME:
    {
        Tarot::Game game;
        in >> game;
        arg.AddValue("mode", game.mode);
        command = "NewGame";
        break;
    }

    case Protocol::TABLE_NEW_DEAL:
    {
        Deck deck;
        in >> deck;
        arg.AddValue("cards", deck.ToString());
        command = "NewDeal";
        break;
    }

    case Protocol::TABLE_REQUEST_BID:
    {
        Contract c;
        Place p;

        in >> c; // Most important contract announced before
        in >> p;
        arg.AddValue("place", p.ToString());
        arg.AddValue("contract", c.ToString());
        command = "RequestBid";
        break;
    }

    case Protocol::TABLE_SHOW_PLAYER_BID:
    {
        Place p;
        Contract c;
        bool slam;

        in >> p;
        in >> c;
        in >> slam;

        arg.AddValue("place", p.ToString());
        arg.AddValue("contract", c.ToString());
        arg.AddValue("slam", slam);
        command = "ShowBid";
        break;
    }

    case Protocol::TABLE_ALL_PASSED:
    {
        command = "AllPassed";
        break;
    }

    case Protocol::TABLE_SHOW_DOG:
    {
        Deck dog;
        in >> dog;
        arg.AddValue("dog", dog.ToString());
        command = "ShowDog";
        break;
    }

    case Protocol::TABLE_ASK_FOR_DISCARD:
    {
        command = "BuildDiscard";
        break;
    }

    case Protocol::TABLE_START_DEAL:
    {
        Place first;
        Tarot::Bid bid;
        Tarot::Distribution shuffle;
        in >> first;
        in >> bid.taker;
        in >> bid.contract;
        in >> bid.slam;
        in >> shuffle;

        arg.AddValue("first_player", first.ToString());
        arg.AddValue("taker", bid.taker.ToString());
        arg.AddValue("contract", bid.contract.ToString());
        arg.AddValue("slam", bid.slam);
        command = "StartDeal";
        break;
    }

    case Protocol::TABLE_ASK_FOR_HANDLE:
    {
        command = "AskForHandle";
        break;
    }

    case Protocol::TABLE_SHOW_HANDLE:
    {
        Place p;
        Deck handle;
        in >> p;
        in >> handle;
        arg.AddValue("place", p.ToString());
        arg.AddValue("handle", handle.ToString());
        command = "ShowHandle";
        break;
    }

    case Protocol::TABLE_SHOW_CARD:
    {
        std::string name;
        Place player;

        in >> player;
        in >> name;

        arg.AddValue("place", player.ToString());
        arg.AddValue("card", name);
        command = "ShowCard";
        break;
    }

    case Protocol::TABLE_PLAY_CARD:
    {
        Place p;
        in >> p;

        arg.AddValue("place", p.ToString());
        command = "PlayCard";
        break;
    }

    case Protocol::TABLE_END_OF_TRICK:
    {
        Place winner;
        in >> winner;

        arg.AddValue("place", winner.ToString());
        command = "EndOfTrick";
        break;
    }

    case Protocol::TABLE_END_OF_DEAL:
    {
        Points points;
        std::string result;
        in >> points;
        in >> result;

        JsonValue json;
        if (JsonReader::ParseString(json, result))
        {
            command = "EndOfDeal";
            arg.AddValue("result", json.GetObject());           
            arg.AddValue("points", points.pointsAttack);
            arg.AddValue("score", points.scoreAttack);
            arg.AddValue("oudlers", points.oudlers);
            arg.AddValue("little_bonus", points.littleEndianPoints);
            arg.AddValue("handle_bonus", points.handlePoints);
            arg.AddValue("slam_bonus", points.slamPoints);
        }
        break;
    }

    case Protocol::TABLE_END_OF_GAME:
    {
        Place winner;
        in >> winner;

        arg.AddValue("winner", winner.ToString());
        command = "EndOfGame";
        break;
    }

    default:
        break;
    }

    if (command.size() > 0)
    {
        arg.AddValue("cmd", command);
        mEventHandler.EmitEvent(arg.ToString(0U));
    }

    if (error != 0U)
    {
        mEventHandler.EmitError(error);
    }

    return ret;
}


//=============================================================================
// End of file JsonClient.cpp
//=============================================================================
