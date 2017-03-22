/*=============================================================================
 * TarotClub - Lobby.h
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

#ifndef LOBBY_H
#define LOBBY_H

// Tarot files
#include "Protocol.h"
#include "PlayingTable.h"
#include "Users.h"
#include "Network.h"

// ICL files
#include "Observer.h"

/*****************************************************************************/
class Lobby : public net::IEvent
{

public:
    static const std::uint32_t cErrorFull           = 0U;
    static const std::uint32_t cErrorNickNameUsed   = 1U;
    static const std::uint32_t cErrorTableIdUnknown = 2U;

    Lobby(bool adminMode = false);
    ~Lobby();

    void Initialize(const std::string &name, const std::vector<std::string> &tables);
    std::string GetName() { return mName; }
    void RegisterListener(Observer<JsonValue> &obs);

    // From IEvent
    void Signal(std::uint32_t sig);
    bool Deliver(std::uint32_t src_uuid, std::uint32_t dest_uuid, const std::string &arg, std::vector<Reply> &out);
    std::uint32_t AddUser(std::vector<Reply> &out);
    void RemoveUser(std::uint32_t uuid, std::vector<Reply> &out);
    std::uint32_t GetUuid() { return Protocol::LOBBY_UID; }

    // Users management
    std::uint32_t GetNumberOfPlayers();
    void RemoveAllUsers();

    // Tables management
    std::uint32_t CreateTable(const std::string &tableName, const Tarot::Game &game = Tarot::Game());
    bool DestroyTable(std::uint32_t id);
    void DeleteTables();

private:
    bool mInitialized;
    std::vector<PlayingTable *> mTables;
    UniqueId    mTableIds;
    UniqueId    mUserIds;
    UniqueId    mStagingIds; // staging area where users are in the process to be connected
    Users       mUsers;
    std::string mName;
    bool mAdminMode;
    std::uint32_t mEvCounter;
    Subject<JsonValue> mSubject;

    std::string GetTableName(const std::uint32_t tableId);
    void RemovePlayerFromTable(std::uint32_t uuid, std::uint32_t tableId, std::vector<Reply> &out);
    void SendPlayerList(uint32_t uuid, std::vector<Reply> &out);
    void Error(std::uint32_t error, std::uint32_t dest_uuid, std::vector<Reply> &out);
    JsonObject PlayerStatus(std::uint32_t uuid);
    void SendPlayerEvent(std::uint32_t uuid, const std::string &event, std::vector<Reply> &out);
};

#endif // LOBBY_H
