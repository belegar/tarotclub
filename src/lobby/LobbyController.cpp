#include "LobbyController.h"
#include "Log.h"
#include <sstream>

LobbyController::LobbyController()
    : mPacketNotifier(nullptr)
    , mInitialized(false)
    , mTableIds(Protocol::TABLES_UID, Protocol::TABLES_UID + Protocol::MAXIMUM_TABLES)
{

}
/*****************************************************************************/
LobbyController::~LobbyController()
{
    mTablesMutex.lock();
    // Kill tables
    for (std::vector<Controller *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
    {
        delete (*iter);
    }
    mTablesMutex.unlock();
}
/*****************************************************************************/
void LobbyController::Initialize(const std::string &name, const std::vector<std::string> &tables, IPacketNotifier *notifier)
{
    mPacketNotifier = notifier;
    mName = name;
    for (std::uint32_t i = 0U; i < tables.size(); i++)
    {
        CreateTable(tables[i]);
    }
}
/*****************************************************************************/
std::uint32_t LobbyController::GetNumberOfPlayers()
{
    return mUsers.GetLobbyUsers().size();
}
/*****************************************************************************/
uint32_t LobbyController::AddUser()
{
    return mUsers.AddUser();
}
/*****************************************************************************/
void LobbyController::RemoveUser(uint32_t uuid)
{
    std::uint32_t tableId = mUsers.GetPlayerTable(uuid);
    if (tableId != 0U)
    {
        RemovePlayerFromTable(uuid, tableId);
    }
    // Remove the player from the server
    mUsers.RemoveUser(uuid);

    std::stringstream ss;
    ss << "Player " << mUsers.GetIdentity(uuid).nickname << " has quit the server. UUID: " << (int)uuid;
    TLogNetwork(ss.str());
}
/*****************************************************************************/
void LobbyController::RemoveAllUsers()
{

}
/*****************************************************************************/
std::uint32_t LobbyController::CreateTable(const std::string &tableName, bool adminMode, const Tarot::Game &game)
{
    mTablesMutex.lock();

    std::uint32_t id = mTableIds.TakeId();

    if (id > 0U)
    {
        std::cout << "Creating table \"" << tableName << "\": id=" << id << std::endl;
        Controller *table = new Controller(*this);
        table->SetId(id);
        table->SetName(tableName);
        table->SetAdminMode(adminMode);
        table->SetupGame(game);
        table->Initialize();
        table->CreateTable(4U);
        mTables.push_back(table);
    }
    else
    {
        TLogError("Cannot create table: maximum number of tables reached.");
    }
    mTablesMutex.unlock();

    return id;
}
/*****************************************************************************/
bool LobbyController::DestroyTable(std::uint32_t id)
{
    bool ret = false;
    mTablesMutex.lock();
    for (std::vector<Controller *>::iterator it = mTables.begin(); it != mTables.end(); ++it)
    {
        if ((*it)->GetId() == id)
        {
            delete *it; // Delete the Controller object in heap
            mTables.erase(it);
            ret = true;
            break;
        }
    }
    mTableIds.ReleaseId(id);
    mTablesMutex.unlock();
    return ret;
}
/*****************************************************************************/
bool LobbyController::DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data)
{
    bool ret = true;
    ByteStreamReader in(data);

    // Filter using the destination uuid (table or lobby?)
    if (mTableIds.IsTaken(dest_uuid))
    {
        // gets the table of the sender
        std::uint32_t tableId = mUsers.GetPlayerTable(src_uuid);
        if (tableId == dest_uuid)
        {
            mTablesMutex.lock();
            // forward it to the suitable table controller
            for (std::vector<Controller *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
            {
                if ((*iter)->GetId() == tableId)
                {
                    (*iter)->ExecuteRequest(cmd, src_uuid, dest_uuid, data);
                }
            }
            mTablesMutex.unlock();
        }
        else
        {
            TLogNetwork("Packet received for an invalid table, or player is not connected to the table");
        }
    }
    else if (dest_uuid == Protocol::LOBBY_UID)
    {
        switch (cmd)
        {
        case Protocol::CLIENT_CHAT_MESSAGE:
        {
            std::string message;
            std::uint32_t target;
            in >> message;
            in >> target;

            message = mUsers.GetIdentity(src_uuid).nickname + "> " + message;
            if (mTableIds.IsTaken(target))
            {
                SendData(Protocol::LobbyChatMessage(message, target));
            }
            else
            {
                // Otherwise, send the data to the players connected in the lobby
                SendData(Protocol::LobbyChatMessage(message, Protocol::LOBBY_UID));
            }
            break;
        }

        case Protocol::CLIENT_REPLY_LOGIN:
        {
            Identity ident;
            in >> ident;

            if (mUsers.AccessGranted(src_uuid, ident))
            {
                // Create a list of tables available on the server
                std::map<std::string, std::uint32_t> list;

                mTablesMutex.lock();
                for (std::vector<Controller *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
                {
                    list[(*iter)->GetName()] = (*iter)->GetId();
                }
                mTablesMutex.unlock();

                SendData(Protocol::LobbyLoginResult(true, list, src_uuid));
                std::string message = "The player " + ident.nickname + " has joined the server.";
                TLogNetwork(message);
                SendData(Protocol::LobbyChatMessage(message, Protocol::LOBBY_UID));
                SendData(Protocol::LobbyPlayersList(mUsers.GetLobbyUserNames()));
            }
            break;
        }

        case Protocol::CLIENT_JOIN_TABLE:
        {
            std::uint32_t tableId;
            in >> tableId;

            // A user can join a table if he is _NOT_ already around a table
            if (mUsers.GetPlayerTable(src_uuid) == Protocol::NO_TABLE)
            {
                Place assignedPlace;
                std::uint8_t nbPlayers;
                mTablesMutex.lock();

                // Forward it to the table controller
                for (std::vector<Controller *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
                {
                    if ((*iter)->GetId() == tableId)
                    {
                        assignedPlace = (*iter)->AddPlayer(src_uuid, nbPlayers);
                    }
                }
                mTablesMutex.unlock();

                if (assignedPlace.IsValid())
                {
                    mUsers.SetPlayingTable(src_uuid, tableId, assignedPlace);

                    // New player connected, send table information
                    SendData(Protocol::LobbyJoinTableReply(
                                               true,
                                               assignedPlace,
                                               nbPlayers,
                                               src_uuid,
                                               tableId));

                    std::string msg = "Player " + mUsers.GetIdentity(src_uuid).nickname + " is entering on table: " + GetTableName(tableId);
                    TLogNetwork(msg);

                    SendData(Protocol::LobbyChatMessage(msg, Protocol::LOBBY_UID));

                    // Update player list
                    SendData(Protocol::TablePlayersList(mUsers.GetTablePlayers(tableId), tableId));
                }
                else
                {
                    // Player is already connected, send error
                    SendData(Protocol::LobbyJoinTableReply(
                                               false,
                                               Place::NOWHERE,
                                               0U,
                                               src_uuid,
                                                tableId));

                }
            }
            break;
        }

        case Protocol::CLIENT_QUIT_TABLE:
        {
            std::uint32_t tableId;
            in >> tableId;
            if (mUsers.GetPlayerTable(src_uuid) == tableId)
            {
                RemovePlayerFromTable(src_uuid, tableId);
            }
            break;
        }

        case Protocol::CLIENT_CHANGE_IDENTITY:
        {
            Identity newIdent;
            in >> newIdent;

            mUsers.AccessGranted(src_uuid, newIdent);

            if (mUsers.IsHere(src_uuid))
            {
                // Update player list
                std::uint32_t tableId = mUsers.GetPlayerTable(src_uuid);
                if (tableId != 0U)
                {
                    SendData(Protocol::TablePlayersList(mUsers.GetTablePlayers(tableId), tableId));
                }
                SendData(Protocol::LobbyPlayersList(mUsers.GetLobbyUserNames()));
            }
            break;
        }

        default:
            TLogNetwork("Lobby received a bad packet");
            break;
        }
    }
    else
    {
        std::stringstream ss;
        ss << "Packet destination must be the table or the lobby, nothing else; received UID: " << dest_uuid;
        TLogNetwork(ss.str());
    }

    return ret;
}
/*****************************************************************************/
void LobbyController::RemovePlayerFromTable(std::uint32_t uuid, std::uint32_t tableId)
{
    bool removeAllPlayers = false;
    mTablesMutex.lock();
    // Forward it to the table controller
    for (std::vector<Controller *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
    {
        if ((*iter)->GetId() == tableId)
        {
            removeAllPlayers = (*iter)->RemovePlayer(uuid);
        }
    }
    mTablesMutex.unlock();

    std::list<std::uint32_t> peers;

    if (removeAllPlayers)
    {
        peers = mUsers.GetUsersOfTable(tableId);
    }
    else
    {
        // Remove only one player
        peers.push_back(uuid);
    }

    for (std::list<std::uint32_t>::iterator iter = peers.begin(); iter != peers.end(); ++iter)
    {
        if (mUsers.IsHere(*iter))
        {
            std::stringstream ss;
            ss << "Player " << mUsers.GetIdentity(*iter).nickname << " is leaving table: " << GetTableName(tableId);
            TLogNetwork(ss.str());
            mUsers.SetPlayingTable(*iter, 0U, Place::NOWHERE);
            SendData(Protocol::LobbyChatMessage(ss.str(), Protocol::LOBBY_UID));
            SendData(Protocol::TableQuitEvent(*iter,tableId));
        }
    }

    // Update player list
    SendData(Protocol::TablePlayersList(mUsers.GetTablePlayers(tableId), tableId));
}
/*****************************************************************************/
std::string LobbyController::GetTableName(const std::uint32_t tableId)
{
    std::string name = "error_table_not_found";

    mTablesMutex.lock();
    // Forward it to the table controller
    for (std::vector<Controller *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
    {
        if ((*iter)->GetId() == tableId)
        {
            name = (*iter)->GetName();
        }
    }
    mTablesMutex.unlock();

    return name;
}
/*****************************************************************************/
void LobbyController::SendData(const ByteArray &block)
{
    std::uint32_t dest_uuid = Protocol::GetDestUuid(block);

    std::list<std::uint32_t> peers; // list of users to send the data

    if (mTableIds.IsTaken(dest_uuid))
    {
        // If the player is connected to a table, send data to the table players only
        peers = mUsers.GetUsersOfTable(dest_uuid);
    }
    else if (dest_uuid == Protocol::LOBBY_UID)
    {
        // Send data to all the connected users
        peers = mUsers.GetLobbyUsers();
    }
    else if (mUsers.IsHere(dest_uuid))
    {
        // Only send the data to one connected client
        peers.push_back(dest_uuid);
    }

    if (mPacketNotifier != nullptr)
    {
        mPacketNotifier->Send(block, peers);
    }
}
