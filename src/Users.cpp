/*=============================================================================
 * TarotClub - Users.cpp
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

#include <sstream>
#include "Users.h"

/*****************************************************************************/
Users::Users()
{
}
/*****************************************************************************/
/**
 * @brief Lobby::PlayerTable
 *
 * Gets the player table id, return zero if not playing (in lobby)
 *
 * @param uuid
 * @return
 */
std::uint32_t Users::GetPlayerTable(std::uint32_t uuid)
{
   std::uint32_t tableId = Protocol::LOBBY_UID;

   for (std::uint32_t i = 0; i < mUsers.size(); i++)
   {
       if (mUsers[i].uuid == uuid)
       {
            tableId = mUsers[i].tableId;
            break;
       }
   }
   return tableId;
}
/*****************************************************************************/
bool Users::GetEntry(std::uint32_t uuid, Entry &entry)
{
    bool ret = false;
    for (std::uint32_t i = 0; i < mUsers.size(); i++)
    {
        if (mUsers[i].uuid == uuid)
        {
             entry = mUsers[i];
             ret = true;
             break;
        }
    }
    return ret;
}
/*****************************************************************************/
void Users::Clear()
{
    mUsers.clear();
}
/*****************************************************************************/
void Users::SetPlayingTable(std::uint32_t uuid, std::uint32_t tableId, Place place)
{
    for (std::uint32_t i = 0; i < mUsers.size(); i++)
    {
        if (mUsers[i].uuid == uuid)
        {
             mUsers[i].tableId = tableId;
             mUsers[i].place = place;
             break;
        }
    }
}
/*****************************************************************************/
std::vector<std::uint32_t> Users::GetTablePlayers(std::uint32_t tableId)
{
    std::vector<std::uint32_t> theList;
    for (std::uint32_t i = 0; i < mUsers.size(); i++)
    {
        if (mUsers[i].tableId == tableId)
        {
            theList.push_back(mUsers[i].uuid);
        }
    }
    return theList;
}
/*****************************************************************************/
std::vector<uint32_t> Users::GetAll()
{
    std::vector<std::uint32_t> theList;
    for (std::uint32_t i = 0; i < mUsers.size(); i++)
    {
        theList.push_back(mUsers[i].uuid);
    }
    return theList;
}
/*****************************************************************************/
std::vector<Users::Entry> Users::Get(std::uint32_t filterId)
{
    std::vector<Entry> theList;
    for (std::uint32_t i = 0; i < mUsers.size(); i++)
    {
        if ((mUsers[i].tableId == filterId) ||
            (filterId == Protocol::LOBBY_UID))
        {
            theList.push_back(mUsers[i]);
        }
    }
    return theList;
}
/*****************************************************************************/
uint32_t Users::Size()
{
    return static_cast<std::uint32_t>(mUsers.size());
}
/*****************************************************************************/
bool Users::IsHere(std::uint32_t uuid)
{
    bool isHere = false;
    for (std::uint32_t i = 0; i < mUsers.size(); i++)
    {
        if (mUsers[i].uuid == uuid)
        {
             isHere = true;
             break;
        }
    }
    return isHere;
}
/*****************************************************************************/
bool Users::CheckNickName(std::uint32_t uuid, const std::string &nickname)
{
    // Check if not already used
    bool already_used = false;
    for (std::uint32_t i = 0U; i < mUsers.size(); i++)
    {
        if (mUsers[i].uuid != uuid)
        {
            if (mUsers[i].identity.nickname == nickname)
            {
                already_used = true;
                break;
            }
        }
    }

    return already_used;
}
/*****************************************************************************/
bool Users::UpdateLocation(std::uint32_t uuid, uint32_t tableId, Place p)
{
    bool ret = false;
    for (std::uint32_t i = 0U; i < mUsers.size(); i++)
    {
        if (mUsers[i].uuid == uuid)
        {
             mUsers[i].tableId = tableId;
             mUsers[i].place = p;
             ret = true;
             break;
        }
    }
    return ret;
}
/*****************************************************************************/
bool Users::ChangeNickName(uint32_t uuid, const std::string &nickname)
{
    bool ret = false;

    // Check if not already used before changing it
    if (!CheckNickName(uuid, nickname))
    {
        for (std::uint32_t i = 0U; i < mUsers.size(); i++)
        {
            if (mUsers[i].uuid == uuid)
            {
                 mUsers[i].identity.nickname = nickname;
                 ret = true;
                 break;
            }
        }
    }
    return ret;
}
/*****************************************************************************/
/**
 * @brief Users::AddUser
 *
 * Add a user in the staging area while the login is processing
 *
 * @return
 */
bool Users::AddEntry(const Entry &entry)
{
    bool valid = false;

    if (!IsHere(entry.uuid))
    {
        if (!CheckNickName(entry.uuid, entry.identity.nickname))
        {
            mUsers.push_back(entry);
            valid = true;
        }
    }
    return valid;
}
/*****************************************************************************/
void Users::Remove(std::uint32_t uuid)
{
    for (std::uint32_t i = 0U; i < mUsers.size(); i++)
    {
        if (mUsers[i].uuid == uuid)
        {
            mUsers.erase(mUsers.begin() + i);
            break;
        }
    }
}

//=============================================================================
// End of file Users.cpp
//=============================================================================
