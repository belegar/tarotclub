#include "BotManager.h"

/*****************************************************************************/
BotManager::BotManager()
    : mBotsIds(0U, 10000U)
{

}
/*****************************************************************************/
BotManager::~BotManager()
{

}
/*****************************************************************************/
void BotManager::Close()
{
    const std::lock_guard<std::mutex> lock(mMutex);
    // Close local bots
    for (auto &b : mBots)
    {
        b.second->mSession.Close();
    }
}
/*****************************************************************************/
void BotManager::KillBots()
{
    const std::lock_guard<std::mutex> lock(mMutex);
    for (auto &b : mBots)
    {
        b.second.reset();
    }
}
/*****************************************************************************/
bool BotManager::JoinTable(uint32_t botId, uint32_t tableId)
{
    bool ret = false;
    (void) botId;
    (void) tableId;

    // FIXME: send asynchronous message to the server to join a specific table
    /*
    mMutex.lock();

    if (mBots.count(botId) > 0)
    {
        mBots[botId]->mBot.(tableId);
        ret = true;
    }
    mMutex.unlock();
    */
    return ret;
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
 * @return bot ID
 */
std::uint32_t BotManager::AddBot(std::uint32_t tableToJoin, const Identity &ident, std::uint16_t delay, const std::string &scriptFile)
{
    const std::lock_guard<std::mutex> lock(mMutex);

    std::uint32_t botid = mBotsIds.TakeId();
    if (mBots.count(botid) > 0)
    {
        TLogError("Internal problem, bot id exists");
    }
    mBots[botid] = std::make_unique<NetBot>();

    // Initialize the bot
    mBots[botid]->mBot.SetIdentity(ident);
    mBots[botid]->mBot.SetTimeBeforeSend(delay);
    mBots[botid]->mBot.SetTableToJoin(tableToJoin);
    mBots[botid]->mBot.SetAiScript(scriptFile);

    // Initialize the session
    mBots[botid]->mSession.Initialize("aPdSgVkYp3s6v9y$"); // FIXME: need a proper key
    return botid;
}
/*****************************************************************************/
bool BotManager::ConnectBot(std::uint32_t botId, const std::string &ip, uint16_t port)
{
    bool ret = false;
    const std::lock_guard<std::mutex> lock(mMutex);

    // Connect the bot to the server
    if (mBots.count(botId) > 0)
    {
        mBots[botId]->mSession.ConnectToHost(ip, port);
        ret = true;
    }

    return ret;
}
/*****************************************************************************/
/**
 * @brief BotManager::RemoveBot
 *
 * Removes a bot that belongs to a table. Also specify a place (south, north ...)
 *
 * @param tableId
 * @param p
 * @return
 */
bool BotManager::RemoveBot(std::uint32_t botid)
{
    const std::lock_guard<std::mutex> lock(mMutex);
    bool ret = false;

    if (mBots.count(botid) > 0U)
    {
        // Gracefully close the bot from the server
        mBots[botid]->mSession.Close();
        // delete the object
        mBots[botid].reset();
        // Remove it from the list
        mBots.erase(botid);
        ret = true;
    }
    mBotsIds.ReleaseId(botid);

    return ret;
}
/*****************************************************************************/

/*
void BotManager::ChangeBotIdentity(std::uint32_t uuid, const Identity &identity)
{
    for (std::map<std::uint32_t, NetBot *>::iterator iter = mBots.begin(); iter != mBots.end(); ++iter)
    {
        if (iter->second->mBot.GetUuid() == uuid)
        {
            iter->second->mBot.SetIdentity(identity);
        }
    }
}
*/

