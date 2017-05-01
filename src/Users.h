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
#include "Protocol.h"
#include <vector>
#include <map>

/*****************************************************************************/
class Users
{
public:
    struct Entry
    {
        Entry()
            : uuid(Protocol::INVALID_UID)
            , tableId(Protocol::INVALID_UID)
        {

        }

        std::uint32_t uuid;
        std::uint32_t tableId;  // zero if not playing
        Place place;            // place around the table (if joined a table)
        Identity identity;
    };

    Users();

    // Accessors
    bool IsHere(std::uint32_t uuid);
    std::uint32_t GetPlayerTable(std::uint32_t uuid);
    bool GetEntry(std::uint32_t uuid, Entry &entry);
    void Clear();
    bool CheckNickName(std::uint32_t uuid, const std::string &nickname);
    bool UpdateLocation(uint32_t uuid, std::uint32_t tableId, Place p);
    std::vector<uint32_t> GetTablePlayers(std::uint32_t tableId);
    std::vector<uint32_t> GetAll();
    std::vector<Entry> Get(uint32_t filterId);
    std::uint32_t Size();

    // Mutators
    bool ChangeNickName(std::uint32_t uuid, const std::string &nickname);
    bool AddEntry(const Entry &entry);
    void Remove(std::uint32_t uuid);
    void SetPlayingTable(std::uint32_t uuid, std::uint32_t tableId, Place place);

private:
    std::vector<Entry> mUsers;  // connected players
};

#endif // USERS_H

//=============================================================================
// End of file Users.h
//=============================================================================
