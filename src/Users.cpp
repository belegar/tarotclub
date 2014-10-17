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
std::uint32_t Users::PlayerTable(std::uint32_t uuid)
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
void Users::NewStagingUser(int socket)
{
    std::lock_guard<std::mutex> lock(mMutex);
    // Add the player to the list
    mStaging.push_back(socket);
}
/*****************************************************************************/
void Users::AccessGranted(std::int32_t socket)
{
    std::lock_guard<std::mutex> lock(mMutex);

    std::uint32_t uuid = mIdManager.TakeId();
    // Add the user to the main users list
    mUsers[uuid].socket = socket;
    mUsers[uuid].tableId = 0U;

    if (std::find(mStaging.begin(), mStaging.end(), socket) != mStaging.end())
    {
        mStaging.erase(std::find(mStaging.begin(), mStaging.end(), socket));
    }
    else
    {
        TLogError("User must be present in staging list");
    }
}
/*****************************************************************************/
void Users::RemoveUser(int socket)
{
    std::lock_guard<std::mutex> lock(mMutex);

    // Remove it if he belongs to the staging list
    if (std::find(mStaging.begin(), mStaging.end(), socket) != mStaging.end())
    {
        mStaging.erase(std::find(mStaging.begin(), mStaging.end(), socket));
    }

    for (std::map<std::uint32_t, std::int32_t>::iterator iter = mUsers.begin(); iter != mUsers.end(); ++iter)
    {
        if (iter->second == socket)
        {
            mUsers.erase(iter->first);
            break;
        }
    }
}
/*****************************************************************************/
/**
 * @brief Table::AddBot
 *
 * Add a bot player to a table. Each bot is a Tcp client that connects to the
 * table immediately.
 *
 * @param p
 * @param ident
 * @param delay
 * @return
 */
bool Users::AddBot(std::uint32_t tableToJoin, const Identity &ident, std::uint16_t delay)
{
    std::lock_guard<std::mutex> lock(mBotsMutex);
    bool ret = false;

    if (mBots.find(p) == mBots.end())
    {
        // Place is free (not found), we dynamically add our bot here
        Bot *bot = new Bot();
        // Add it to the list (save the pointer to the allocated object)
        mBots[p] = bot;
        // Initialize the bot
        bot->SetIdentity(ident);
        bot->SetTimeBeforeSend(delay);
        bot->SetTableToJoin(tableToJoin);
        bot->Initialize();
        // Connect the bot to the server
        bot->ConnectToHost("127.0.0.1", mTcpPort);
        ret = true;
    }

    return ret;
}
/*****************************************************************************/

// FIXME: how to identity the bot (id? identity? name?)
bool Users::RemoveBot(Place p)
{
    std::lock_guard<std::mutex> lock(mBotsMutex);
    bool ret = false;

    /*

    std::map<Place, Bot *>::iterator iter = mBots.find(p);
    if (iter != mBots.end())
    {
        // Gracefully close the bot from the server
        iter->second->Close();
        // delete the object
        delete iter->second;
        // Remove it from the list
        mBots.erase(iter);
        ret = true;
    }
    */
    return ret;
}
