#include "Users.h"
#include "Log.h"
#include "Protocol.h"

static const std::uint32_t MAXIMUM_USERS    = 1000U;

/*****************************************************************************/
Users::Users()
    : mIdManager(Protocol::USERS_UID, MAXIMUM_USERS)
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
   std::uint32_t tableId = 0U;

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
void Users::SetPlayingTable(std::uint32_t uuid, std::uint32_t tableId)
{
    std::lock_guard<std::mutex> lock(mMutex);

    if (mUsers.find(uuid) != mUsers.end())
    {
        mUsers[uuid].tableId = tableId;
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
            theList.push_back(iter->second.socket);
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
        if (iter->second.tableId == Protocol::NO_TABLE)
        {
            theList.push_back(iter->second.socket);
        }
    }
    return theList;
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
    mUsers[uuid].tableId = 0U;
    mUsers[uuid].staging = true;

    return uuid;
}
/*****************************************************************************/
bool Users::AccessGranted(std::uint32_t uuid, const Identity &ident)
{
    std::lock_guard<std::mutex> lock(mMutex);
    bool ret = false;
    if (mUsers.find(uuid) != mUsers.end())
    {
        mUsers[uuid].staging = false;
        mUsers[uuid].identity = ident;
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
    }
    else
    {
        TLogError("Bad UUID");
    }
}

