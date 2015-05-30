#ifndef LOBBYCONTROLLER_H
#define LOBBYCONTROLLER_H

#include "Protocol.h"
#include "Controller.h"
#include "Users.h"
#include "Bot.h"
#include "Users.h"

/*****************************************************************************/
class LobbyController : public Protocol::IWorkItem, private Controller::IData
{

public:
    class IPacketNotifier
    {
    public:
        virtual void Send(const ByteArray &data, std::list<std::uint32_t> peers) = 0;
    };

    LobbyController();
    ~LobbyController();

    void Initialize(const std::string &name, const std::vector<std::string> &tables, IPacketNotifier *notifier);
    std::string GetName() { return mName; }

    // Users management
    std::uint32_t GetNumberOfPlayers();
    std::uint32_t AddUser();
    void RemoveUser(std::uint32_t uuid);
    void RemoveAllUsers();

    // Tables management
    std::uint32_t CreateTable(const std::string &tableName, bool adminMode = true, const Tarot::Game &game = Tarot::Game());
    bool DestroyTable(std::uint32_t id);

private:
    IPacketNotifier *mPacketNotifier;
    bool mInitialized;
    std::vector<Controller *> mTables;
    UniqueId    mTableIds;
    std::mutex  mTablesMutex;
    Users       mUsers;
    std::string mName;

    // From Protocol::WorkItem
    bool DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data);

    std::string GetTableName(const std::uint32_t tableId);
    void RemovePlayerFromTable(std::uint32_t uuid, std::uint32_t tableId);
    void SendData(const ByteArray &block);
};

#endif // LOBBYCONTROLLER_H
