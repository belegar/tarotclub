#ifndef USERS_H
#define USERS_H

#include "UniqueId.h"
#include "Identity.h"
#include <map>

class Users
{
public:
    struct Entry
    {
        std::int32_t socket;    // valid if > 0
        std::uint32_t tableId;  // zero if not playing
        bool staging;           // true if the user is in staging (login process)
        Identity identity;
    };

    Users();

    // Players management
    bool IsValid(std::uint32_t uuid, int socket);
    std::uint32_t NewStagingUser(int socket);
    bool AccessGranted(std::uint32_t uuid, const Identity &ident);
    void RemoveUser(std::uint32_t uuid);
    std::uint32_t GetPlayerTable(std::uint32_t uuid);
    std::int32_t GetSocket(std::uint32_t uuid);
    Identity GetIdentity(std::uint32_t uuid);
    std::uint32_t GetUuid(std::int32_t socket);

    void SetPlayingTable(std::uint32_t uuid, std::uint32_t tableId);

    // Get some lists of specific group
    std::list<std::uint32_t> GetUsersOfTable(std::uint32_t tableId);
    std::list<std::uint32_t> GetLobbyUsers();


    void CloseClients();

private:
    UniqueId mIdManager;
    std::mutex mMutex;
    std::map<std::uint32_t, Entry> mUsers;  // connected players on the server. key = UUID
};

#endif // USERS_H
