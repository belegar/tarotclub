#ifndef USERS_H
#define USERS_H

#include "UniqueId.h"
#include "Bot.h"
#include "Identity.h"
#include <map>

class Users
{
public:
    struct Entry
    {
        std::int32_t socket;    // valid if > 0
        std::uint32_t tableId;  // zero if not playing
        Identity identity;
    };

    Users();

    // Bots management
    bool AddBot(std::uint32_t tableToJoin, const Identity &ident, std::uint16_t delay);
    bool RemoveBot(Place p);

    // Players management
    bool IsValid(std::uint32_t uuid, int socket);
    void NewStagingUser(int socket);
    void AccessGranted(std::int32_t socket);
    void RemoveUser(int socket);

private:
    UniqueId mIdManager;
    std::mutex mMutex;

    // Users management
    std::map<std::uint32_t, Entry> mUsers;  // connected players on the server. key = UUID
    std::list<std::int32_t> mStaging;     // users with login in process. value = socket

    // Bots management
    std::list<Bot *> mBots;
    std::mutex mBotsMutex;
};

#endif // USERS_H
