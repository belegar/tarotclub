#ifndef BOTMANAGER_H
#define BOTMANAGER_H

#include "Session.h"
#include "Bot.h"
#include "UniqueId.h"
#include "Identity.h"

class BotManager
{
public:
    BotManager();
    ~BotManager();

    std::uint32_t AddBot(std::uint32_t tableToJoin, const Identity &ident, std::uint16_t delay, const std::string &scriptFile);
    bool ConnectBot(uint32_t botId, const std::string &ip, std::uint16_t port);
    bool RemoveBot(std::uint32_t botid);
//    void ChangeBotIdentity(std::uint32_t uuid, const Identity &identity);
    void Close();
    void KillBots();
    bool JoinTable(std::uint32_t botId, std::uint32_t tableId);

private:
    struct NetBot : public net::IEvent
    {
        Session mSession;
        Bot mBot;

        NetBot()
         : mSession(*this)
        {

        }

        virtual void Signal(std::uint32_t sig) { (void) sig; }
        virtual bool Deliver(uint32_t src_uuid, uint32_t dest_uuid, const std::string &arg, std::vector<Reply> &out)
        {
            return mBot.Decode(src_uuid, dest_uuid, arg, out);
        }
        virtual std::uint32_t AddUser(std::vector<Reply> &/*out*/) { return 0U; /* stub */ }
        virtual void RemoveUser(std::uint32_t /* uuid */, std::vector<Reply> &/*out*/) {}
        std::uint32_t GetUuid() { return mBot.GetUuid(); }
    };


    // Bots management
    std::map<std::uint32_t, NetBot *> mBots;
    std::mutex mMutex;
    UniqueId mBotsIds; ///< Internal management of bots only, not related to uuid used by the protocol
};

#endif // BOTMANAGER_H
