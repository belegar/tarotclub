/*=============================================================================
 * TarotClub - Users.h
 *=============================================================================
 * Management of connected users in the lobby, provide utility methods
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

#ifndef USERS_H
#define USERS_H

#include "UniqueId.h"
#include "Identity.h"
#include "Common.h"
#include <map>

/*****************************************************************************/
class Users
{
public:
    struct Entry
    {
        std::int32_t socket;    // valid if > 0
        std::uint32_t tableId;  // zero if not playing
        bool connected;         // true if the user is connected (not in login process)
        Place place;            // place around the table (if joined a table)
        Identity identity;
    };

    Users();

    // Players management
    bool IsValid(std::uint32_t uuid, int socket);
    bool IsHere(std::uint32_t uuid);
    std::uint32_t NewStagingUser(int socket);
    bool AccessGranted(std::uint32_t uuid, const Identity &ident);
    void RemoveUser(std::uint32_t uuid);
    std::uint32_t GetPlayerTable(std::uint32_t uuid);
    std::int32_t GetSocket(std::uint32_t uuid);
    Identity GetIdentity(std::uint32_t uuid);
    std::uint32_t GetUuid(std::int32_t socket);

    void SetPlayingTable(std::uint32_t uuid, std::uint32_t tableId);

    // Get some lists of specific group
    std::list<std::uint32_t> GetUsersOfTable(std::uint32_t tableId);
    std::map<Place, Identity> GetTablePlayers(std::uint32_t tableId);
    std::map<std::uint32_t, std::string> GetLobbyUserNames();
    std::list<std::uint32_t> GetLobbyUsers();

    void CloseClients();

private:
    UniqueId mIdManager;
    std::mutex mMutex;
    std::map<std::uint32_t, Entry> mUsers;  // connected players on the server. key = UUID
};

#endif // USERS_H

//=============================================================================
// End of file Users.h
//=============================================================================
