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

#include "Users.h"
#include "Log.h"
#include "Protocol.h"
#include "TcpSocket.h"

/*****************************************************************************/
Users::Users()
    : mIdManager(Protocol::USERS_UID, Protocol::MAXIMUM_USERS)
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
   std::lock_guard<std::mutex> lock(mMutex);
   std::uint32_t tableId = Protocol::NO_TABLE;

   if ((mUsers.find(uuid) != mUsers.end()))
   {
        tableId = mUsers[uuid].tableId;
   }

   return tableId;
}
/*****************************************************************************/
// Fixme: change the API, return a bool
std::int32_t Users::GetSocket(std::uint32_t uuid)
{
    std::lock_guard<std::mutex> lock(mMutex);
    std::int32_t socket = -1;
    if (mUsers.find(uuid) != mUsers.end())
    {
        socket = mUsers[uuid].socket;
    }
    return socket;
}
/*****************************************************************************/
// Fixme: change the API, return a bool
Identity Users::GetIdentity(std::uint32_t uuid)
{
    std::lock_guard<std::mutex> lock(mMutex);
    Identity ident;
    if (mUsers.find(uuid) != mUsers.end())
    {
        ident = mUsers[uuid].identity;
    }
    return ident;
}
/*****************************************************************************/
std::uint32_t Users::GetUuid(std::int32_t socket)
{
    std::lock_guard<std::mutex> lock(mMutex);
    std::uint32_t uuid = Protocol::INVALID_UID;
    for (std::map<std::uint32_t, Entry>::iterator iter = mUsers.begin(); iter != mUsers.end(); ++iter)
    {
        if (iter->second.socket == socket)
        {
            uuid = iter->first;
        }
    }
    return uuid;
}
/*****************************************************************************/
void Users::SetPlayingTable(std::uint32_t uuid, std::uint32_t tableId, Place place)
{
    std::lock_guard<std::mutex> lock(mMutex);

    if (mUsers.find(uuid) != mUsers.end())
    {
        mUsers[uuid].tableId = tableId;
        mUsers[uuid].place = place;
    }
}
/*****************************************************************************/
std::list<std::uint32_t> Users::GetUsersOfTable(std::uint32_t tableId)
{
    std::lock_guard<std::mutex> lock(mMutex);
    std::list<std::uint32_t> theList;
    for (std::map<std::uint32_t, Entry>::iterator iter = mUsers.begin(); iter != mUsers.end(); ++iter)
    {
        if (iter->second.tableId == tableId)
        {
            theList.push_back(iter->first);
        }
    }
    return theList;
}
/*****************************************************************************/
std::map<Place, Identity> Users::GetTablePlayers(std::uint32_t tableId)
{
    std::lock_guard<std::mutex> lock(mMutex);

    std::map<Place, Identity> theList;
    for (std::map<std::uint32_t, Entry>::iterator iter = mUsers.begin(); iter != mUsers.end(); ++iter)
    {
        if (iter->second.tableId == tableId)
        {
            theList[iter->second.place] = iter->second.identity;
        }
    }
    return theList;
}
/*****************************************************************************/
std::map<std::uint32_t, std::string> Users::GetLobbyUserNames()
{
    std::lock_guard<std::mutex> lock(mMutex);
    std::map<std::uint32_t, std::string> theList;
    for (std::map<std::uint32_t, Entry>::iterator iter = mUsers.begin(); iter != mUsers.end(); ++iter)
    {
        // Do not include users in login process
        if (iter->second.connected)
        {
            theList[iter->first] = iter->second.identity.nickname;
        }
    }
    return theList;
}
/*****************************************************************************/
std::list<std::uint32_t> Users::GetLobbyUsers()
{
    std::lock_guard<std::mutex> lock(mMutex);
    std::list<std::uint32_t> theList;
    for (std::map<std::uint32_t, Entry>::iterator iter = mUsers.begin(); iter != mUsers.end(); ++iter)
    {
        // Do not include users in login process
        if (iter->second.connected)
        {
            theList.push_back(iter->first);
        }
    }
    return theList;
}
/*****************************************************************************/
void Users::CloseClients()
{
    std::lock_guard<std::mutex> lock(mMutex);
    TcpSocket peer;

    for (std::map<std::uint32_t, Entry>::iterator iter = mUsers.begin(); iter != mUsers.end(); ++iter)
    {
        peer.SetSocket(iter->second.socket);
        peer.Close();
    }
    mUsers.clear();
}
/*****************************************************************************/
bool Users::IsValid(std::uint32_t uuid, int socket)
{
    std::lock_guard<std::mutex> lock(mMutex);
    bool valid = false;

    if (mUsers.find(uuid) != mUsers.end())
    {
        if (mUsers[uuid].socket == socket)
        {
            valid = true;
        }
    }
    return valid;
}
/*****************************************************************************/
bool Users::IsHere(std::uint32_t uuid)
{
    std::lock_guard<std::mutex> lock(mMutex);
    return mUsers.find(uuid) != mUsers.end();
}
/*****************************************************************************/
/**
 * @brief Users::NewStagingUser
 *
 * Add a user in the staging area while the login is processing
 *
 * @param socket
 * @return
 */
std::uint32_t Users::NewStagingUser(int socket)
{
    std::lock_guard<std::mutex> lock(mMutex);
    std::uint32_t uuid = mIdManager.TakeId();
    // Add the user to the main users list
    mUsers[uuid].socket = socket;
    mUsers[uuid].tableId = Protocol::NO_TABLE;
    mUsers[uuid].connected = false;

    return uuid;
}
/*****************************************************************************/
bool Users::AccessGranted(std::uint32_t uuid, const Identity &ident)
{
    std::lock_guard<std::mutex> lock(mMutex);
    bool ret = false;
    if (mUsers.find(uuid) != mUsers.end())
    {
        mUsers[uuid].connected = true;
        mUsers[uuid].identity = ident;

        // We change its name if equal to an already connected player
        for (std::map<std::uint32_t, Entry>::iterator iter = mUsers.begin(); iter != mUsers.end(); ++iter)
        {
            if (iter->first != uuid)
            {
                if (iter->second.identity.nickname == ident.nickname)
                {
                    // Append the uuid to the name to make it unique within the server
                    std::stringstream ss;
                    ss << ident.nickname << uuid;
                    mUsers[uuid].identity.nickname =  ss.str();
                }
            }
        }

        ret = true;
    }
    else
    {
        TLogError("User must be present");
    }

    return ret;
}
/*****************************************************************************/
void Users::RemoveUser(std::uint32_t uuid)
{
    std::lock_guard<std::mutex> lock(mMutex);

    // Remove it if he belongs to the staging list
    if (mUsers.find(uuid) != mUsers.end())
    {
        mUsers.erase(uuid);
        mIdManager.ReleaseId(uuid);
    }
    else
    {
        TLogError("Bad UUID");
    }
}

//=============================================================================
// End of file Users.cpp
//=============================================================================
