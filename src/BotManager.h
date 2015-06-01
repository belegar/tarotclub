#ifndef BOTMANAGER_H
#define BOTMANAGER_H

#include "Bot.h"
#include "UniqueId.h"

class BotManager
{
public:
    BotManager();
    ~BotManager();

    std::uint32_t AddBot(std::uint32_t tableToJoin, const Identity &ident, std::uint16_t delay, const std::string &scriptFile);
    bool ConnectBot(uint32_t botId, const std::string &ip, std::uint16_t port);
    bool RemoveBot(std::uint32_t botid);
    void ChangeBotIdentity(std::uint32_t uuid, const Identity &identity);
    void Close();
    void KillBots();

private:
    // Bots management
    std::map<std::uint32_t, Bot *> mBots;
    std::mutex mBotsMutex;
    UniqueId mBotsIds; ///< Internal management of bots only, not related to uuid used by the protocol
};

#endif // BOTMANAGER_H
