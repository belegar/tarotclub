#include "BotManager.h"

/*****************************************************************************/
BotManager::BotManager()
    : mBotsIds(0U, 10000U)
{

}
/*****************************************************************************/
BotManager::~BotManager()
{
    KillBots();
}
/*****************************************************************************/
void BotManager::Close()
{
    mBotsMutex.lock();
    // Close local bots
    for (std::map<std::uint32_t, Bot *>::iterator iter = mBots.begin(); iter != mBots.end(); ++iter)
    {
        iter->second->Close();
    }
    mBotsMutex.unlock();
}
/*****************************************************************************/
void BotManager::KillBots()
{
    // Kill bots
    mBotsMutex.lock();
    for (std::map<std::uint32_t, Bot *>::iterator iter = mBots.begin(); iter != mBots.end(); ++iter)
    {
        delete (iter->second);
    }
    mBotsMutex.unlock();
}
/*****************************************************************************/
bool BotManager::JoinTable(uint32_t botId, uint32_t tableId)
{
    bool ret = false;
    mBotsMutex.lock();
    // Connect the bot to the server
    if (mBots.count(botId) > 0)
    {
        mBots[botId]->JoinTable(tableId);
        ret = true;
    }
    mBotsMutex.unlock();
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
uint32_t BotManager::AddBot(std::uint32_t tableToJoin, const Identity &ident, std::uint16_t delay, const std::string &scriptFile)
{
    // Place is free (not found), we dynamically add our bot here
    Bot *bot = new Bot();

    // Initialize the bot
    bot->SetIdentity(ident);
    bot->SetTimeBeforeSend(delay);
    bot->SetTableToJoin(tableToJoin);
    bot->SetAiScript(scriptFile);
    bot->Initialize();

    mBotsMutex.lock();
    // Add it to the list (save the pointer to the allocated object)
    std::uint32_t botid = mBotsIds.TakeId();
    if (mBots.count(botid) > 0)
    {
        TLogError("Internal problem, bot id exists");
    }
    mBots[botid] = bot;
    mBotsMutex.unlock();

    return botid;
}
/*****************************************************************************/
bool BotManager::ConnectBot(std::uint32_t botId, const std::string &ip, uint16_t port)
{
    bool ret = false;
    mBotsMutex.lock();
    // Connect the bot to the server
    if (mBots.count(botId) > 0)
    {
        mBots[botId]->ConnectToHost(ip, port);
        ret = true;
    }
    mBotsMutex.unlock();
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
    mBotsMutex.lock();
    bool ret = false;

    if (mBots.count(botid) > 0U)
    {
        // Gracefully close the bot from the server
        mBots[botid]->Close();
        // delete the object
        delete (mBots[botid]);
        // Remove it from the list
        mBots.erase(botid);
        ret = true;
    }

    mBotsIds.ReleaseId(botid);
    mBotsMutex.unlock();
    return ret;
}
/*****************************************************************************/
void BotManager::ChangeBotIdentity(std::uint32_t uuid, const Identity &identity)
{
    for (std::map<std::uint32_t, Bot *>::iterator iter = mBots.begin(); iter != mBots.end(); ++iter)
    {
        if (iter->second->GetUuid() == uuid)
        {
            iter->second->SetIdentity(identity);
        }
    }
}

