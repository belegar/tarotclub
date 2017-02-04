/*=============================================================================
 * TarotClub - Lobby.cpp
 *=============================================================================
 * Central meeting point of a server to chat and join game tables
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

// C++ files
#include <sstream>

// Tarot files
#include "Lobby.h"
#include "Network.h"

// ICL files
#include "Log.h"
#include "JsonReader.h"
#include "JsonWriter.h"


/*****************************************************************************/
Lobby::Lobby(bool adminMode)
    : mInitialized(false)
    , mTableIds(Protocol::TABLES_UID, Protocol::TABLES_UID + Protocol::MAXIMUM_TABLES)
    , mAdminMode(adminMode)
{

}
/*****************************************************************************/
Lobby::~Lobby()
{
    // Kill tables
    for (std::vector<PlayingTable *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
    {
        delete (*iter);
    }
}
/*****************************************************************************/
void Lobby::Initialize(const std::string &name, const std::vector<std::string> &tables)
{
    mName = name;
    for (std::uint32_t i = 0U; i < tables.size(); i++)
    {
        CreateTable(tables[i]);
    }
}
/*****************************************************************************/
void Lobby::Signal(uint32_t sig)
{
    (void) sig;
}
/*****************************************************************************/
bool Lobby::Deliver(uint32_t src_uuid, uint32_t dest_uuid, const std::string &arg, std::vector<Reply> &out)
{
    bool ret = true;
    JsonReader reader;
    JsonValue json;

    if (!reader.ParseString(json, arg))
    {
        TLogNetwork("Not a JSON data");
        return false;
    }

    std::string cmd = json.FindValue("cmd").GetString();

    // Filter using the destination uuid (table or lobby?)
    if (mTableIds.IsTaken(dest_uuid))
    {
        // gets the table of the sender
        std::uint32_t tableId = mUsers.GetPlayerTable(src_uuid);
        if (tableId == dest_uuid)
        {
            // forward it to the suitable table PlayingTable
            for (std::vector<PlayingTable *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
            {
                if ((*iter)->GetId() == tableId)
                {
                    (*iter)->ExecuteRequest(cmd, src_uuid, dest_uuid, json, out);
                }
            }
        }
        else
        {
            ret = false;
            TLogNetwork("Packet received for an invalid table, or player is not connected to the table");
        }
    }
    else if (dest_uuid == Protocol::LOBBY_UID)
    {
        if (cmd == "ChatMessage")
        {
            // cmd, source, target elements
            if (json.GetObj().GetSize() == 3U)
            {
                std::uint32_t target = json.FindValue("target").GetInteger();
                out.push_back(Reply(target, json.GetObj()));
            }
        }
        else if (cmd == "ReplyLogin")
        {
            std::string nickname = json.FindValue("nickname").GetString();

            if (mUsers.AccessGranted(src_uuid, nickname))
            {
                // Create a list of tables available on the server
                JsonObject reply;
                JsonArray tables;

                for (std::vector<PlayingTable *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
                {
                    JsonObject table;
                    table.AddValue("name", (*iter)->GetName());
                    table.AddValue("uuid", (*iter)->GetId());
                    tables.AddValue(table);
                }

                reply.AddValue("cmd", "AccessGranted");
                reply.AddValue("tables", tables);

                // Send to the player the final step of the login process
                out.push_back(Reply(src_uuid, reply));

                std::vector<std::uint32_t> peers;
                peers.push_back(src_uuid);

                // Send to all the list of players and the event
                SendPlayerList(peers, "JoinPlayer", out);
            }
            else
            {
                Error(cErrorNickNameUsed, src_uuid, out);
            }
        }
        else if (cmd == "RequestJoinTable")
        {
            std::uint32_t tableId = json.FindValue("table_id").GetInteger();

            // A user can join a table if he is _NOT_ already around a table
            if (mUsers.GetPlayerTable(src_uuid) == Protocol::LOBBY_UID)
            {
                Place assignedPlace;
                std::uint8_t nbPlayers = 0U;
                bool foundTable = false;

                // Forward it to the table PlayingTable
                for (std::vector<PlayingTable *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
                {
                    if ((*iter)->GetId() == tableId)
                    {
                        foundTable = true;
                        assignedPlace = (*iter)->AddPlayer(src_uuid, nbPlayers);
                        break;
                    }
                }

                if (!foundTable)
                {
                    Error(cErrorTableIdUnknown, src_uuid, out);
                }
                else if (assignedPlace.IsValid())
                {
                    mUsers.SetPlayingTable(src_uuid, tableId, assignedPlace);

                    JsonObject reply;

                    reply.AddValue("cmd", "ReplyJoinTable");
                    reply.AddValue("table_id", tableId);
                    reply.AddValue("place", assignedPlace.ToString());
                    reply.AddValue("size", nbPlayers);

                    JsonArray array;

                    std::vector<std::uint32_t> players = mUsers.GetTablePlayers(tableId);

                    for (std::uint32_t i = 0U; i < players.size(); i++)
                    {
                        Users::Entry entry;
                        if (mUsers.GetEntry(players[i], entry))
                        {
                            JsonObject obj;
                            obj.AddValue("uuid", entry.uuid);
                            obj.AddValue("place", entry.place.ToString());
                            array.AddValue(obj);
                        }
                    }

                    reply.AddValue("players", array);
                    out.push_back(Reply(src_uuid, reply));
                }
                else
                {
                    Error(cErrorFull, src_uuid, out);
                }
            }
        }
        else if (cmd == "RequestQuitTable")
        {
            std::uint32_t tableId = json.FindValue("table_id").GetInteger();

            if (mUsers.GetPlayerTable(src_uuid) == tableId)
            {
                RemovePlayerFromTable(src_uuid, tableId, out);
            }
        }
        else if (cmd == "RequestChangeNickname")
        {
            std::string nickname = json.FindValue("nickname").GetString();


            if (mUsers.ChangeNickName(src_uuid, nickname))
            {
                std::vector<std::uint32_t> peers;
                peers.push_back(src_uuid);

                // Send to all the list of players and the event
                SendPlayerList(peers, "ChangeNick", out);
            }
            else
            {
                Error(cErrorNickNameUsed, src_uuid, out);
            }
        }
        else
        {
            ret = false;
            TLogNetwork("Lobby received a bad packet");
        }
    }
    else
    {
        ret = false;
        std::stringstream ss;
        ss << "Packet destination must be the table or the lobby, nothing else; received UID: " << dest_uuid;
        TLogNetwork(ss.str());
    }

    return ret;
}
/*****************************************************************************/
std::uint32_t Lobby::GetNumberOfPlayers()
{
    return mUsers.GetLobbyUsers().size();
}
/*****************************************************************************/
uint32_t Lobby::AddUser(std::vector<Reply> &out)
{
    std::uint32_t uuid = mUsers.AddUser();

    JsonObject json;
    json.AddValue("cmd", "RequestLogin");
    json.AddValue("uuid", uuid);

    out.push_back(Reply(uuid, json));

    return uuid;
}
/*****************************************************************************/
void Lobby::RemoveUser(uint32_t uuid, std::vector<Reply> &out)
{
    std::uint32_t tableId = mUsers.GetPlayerTable(uuid);
    if (tableId != Protocol::LOBBY_UID)
    {
        // First, remove the player from the table
        RemovePlayerFromTable(uuid, tableId, out);
    }
    // Remove the player from the lobby list
    mUsers.RemoveUser(uuid);
}
/*****************************************************************************/
void Lobby::RemoveAllUsers()
{
    mUsers.Clear();
}
/*****************************************************************************/
std::uint32_t Lobby::CreateTable(const std::string &tableName, const Tarot::Game &game)
{
    std::uint32_t id = mTableIds.TakeId();

    if (id > 0U)
    {
        std::stringstream ss;
        ss << "Creating table \"" << tableName << "\": id=" << id << std::endl;
        TLogInfo(ss.str());

        PlayingTable *table = new PlayingTable();
        table->SetId(id);
        table->SetName(tableName);
        table->SetAdminMode(mAdminMode);
        table->SetupGame(game);
        table->Initialize();
        table->CreateTable(4U);
        mTables.push_back(table);
    }
    else
    {
        TLogError("Cannot create table: maximum number of tables reached.");
    }
    return id;
}
/*****************************************************************************/
bool Lobby::DestroyTable(std::uint32_t id)
{
    bool ret = false;
    for (std::vector<PlayingTable *>::iterator it = mTables.begin(); it != mTables.end(); ++it)
    {
        if ((*it)->GetId() == id)
        {
            delete *it; // Delete the PlayingTable object in heap
            mTables.erase(it);
            ret = true;
            break;
        }
    }
    mTableIds.ReleaseId(id);
    return ret;
}
/*****************************************************************************/
void Lobby::Error(std::uint32_t error, std::uint32_t dest_uuid, std::vector<Reply> &out)
{
    static const char* errors[] { "Table is full", "Nickname already used", "Unknown table ID" };
    JsonObject reply;

    reply.AddValue("cmd", "Error");
    reply.AddValue("code", error);

    if (error < (sizeof(errors)/sizeof(errors[0])))
    {
        reply.AddValue("reason", errors[error]);

        out.push_back(Reply(dest_uuid, reply));
    }
}
/*****************************************************************************/
void Lobby::RemovePlayerFromTable(std::uint32_t uuid, std::uint32_t tableId, std::vector<Reply> &out)
{
    bool removeAllPlayers = false;

    // Forward it to the table PlayingTable
    for (std::vector<PlayingTable *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
    {
        if ((*iter)->GetId() == tableId)
        {
            // Remove the player from the table, if we are in game, then all are removed
            removeAllPlayers = (*iter)->RemovePlayer(uuid);
        }
    }

    // Warn one or more player that they are kicked from the table
    std::vector<std::uint32_t> peers;
    if (removeAllPlayers)
    {
        peers = mUsers.GetTablePlayers(tableId);
    }
    else
    {
        // Remove only one player
        peers.push_back(uuid);
    }

    for (std::uint32_t i = 0U; i < peers.size(); i++)
    {
        if (mUsers.IsHere(peers[i]))
        {
            mUsers.SetPlayingTable(peers[i], 0U, Place::NOWHERE); // refresh lobby state
        }
    }

    SendPlayerList(peers, "QuitTable", out);
}
/*****************************************************************************/
void Lobby::SendPlayerList(const std::vector<std::uint32_t> &players, const std::string &event, std::vector<Reply> &out)
{
    std::vector<Users::Entry> users = mUsers.GetLobbyUsers();
    std::vector<std::uint32_t> list;


    JsonObject obj;
    JsonArray array;

    for (uint32_t i = 0U; i < users.size(); i++)
    {
        JsonObject player;
        std::uint32_t uuid = users[i].uuid;
        list.push_back(uuid);

        player.AddValue("uuid", uuid);
        player.AddValue("nickname", users[i].nickname);
        player.AddValue("table", users[i].tableId);

        std::string ev = "none";

        // search for specific event for that list of players
        for (std::uint32_t j = 0U; j < players.size(); j++)
        {
            if (players[j] == users[j].uuid)
            {
                ev = event;
            }
        }
        player.AddValue("event", ev);
        array.AddValue(player);
    }

    obj.AddValue("cmd", "PlayerList");
    obj.AddValue("players", array);

    out.push_back(Reply(list, obj));
}

/*****************************************************************************/
std::string Lobby::GetTableName(const std::uint32_t tableId)
{
    std::string name = "error_table_not_found";

    // Forward it to the table PlayingTable
    for (std::vector<PlayingTable *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
    {
        if ((*iter)->GetId() == tableId)
        {
            name = (*iter)->GetName();
        }
    }

    return name;
}
