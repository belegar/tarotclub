/*=============================================================================
 * TarotClub - Lobby.cpp
 *=============================================================================
 * Manage temporary connections to join free game tables
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 *=============================================================================
 */

#include "Lobby.h"
#include "Log.h"
#include "Util.h"
#include <sstream>
#include <vector>


const std::string Lobby::LOBBY_VERSION_STRING = std::string("TarotClub Lobby v") + std::string("2");

/*****************************************************************************/
Lobby::Lobby()
    : mTcpPort(ServerConfig::DEFAULT_GAME_TCP_PORT)
    , mTcpServer(*this)
    , mTableIds(Protocol::TABLES_UID, Protocol::MAXIMUM_TABLES)
{

}
/*****************************************************************************/
Lobby::~Lobby()
{
    Stop();

    mTablesMutex.lock();
    mBotsMutex.lock();
    // Kill tables
    for (std::vector<Controller *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
    {
        delete (*iter);
    }

    // Kill bots
    for (std::list<Bot *>::iterator iter = mBots.begin(); iter != mBots.end(); ++iter)
    {
        delete (*iter);
    }
    mTablesMutex.unlock();
    mBotsMutex.unlock();
}
/*****************************************************************************/
void Lobby::Initialize(const ServerOptions &opt)
{
    Protocol::GetInstance().Initialize();

    Tarot::Distribution sh;
    sh.mType = Tarot::Distribution::RANDOM_DEAL;

    // Initialize all the tables, starting with the TCP port indicated
    mTcpPort = opt.game_tcp_port;
    for (std::uint32_t i = 0U; i < opt.tables.size(); i++)
    {
        CreateTable(opt.tables[i]);
    }

    // Lobby TCP server
    mTcpServer.Start(opt.game_tcp_port, opt.lobby_max_conn, opt.localHostOnly);
}
/*****************************************************************************/
std::uint32_t Lobby::CreateTable(const std::string &tableName, bool adminMode, const Tarot::Game &game)
{
    mTablesMutex.lock();

    std::uint32_t id = mTableIds.TakeId();

    if (id > 0U)
    {
        std::cout << "Creating table " << tableName << ": id=" << id << std::endl;
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
bool Lobby::DestroyTable(std::uint32_t id)
{
    bool ret = false;
    mTablesMutex.lock();
    for (std::vector<Controller *>::iterator it = mTables.begin(); it != mTables.end(); ++it)
    {
        if ((*it)->GetId() == id)
        {
            mTables.erase(it);
            ret = true;
            break;
        }
    }
    mTablesMutex.unlock();
    return ret;
}
/*****************************************************************************/
void Lobby::WaitForEnd()
{
    mTcpServer.Join();
}
/*****************************************************************************/
void Lobby::NewConnection(int socket)
{
    std::uint32_t uuid = mUsers.NewStagingUser(socket);

    std::stringstream ss;
    ss << "New connection: socket=" << socket
       << ", IP=" << mTcpServer.GetPeerName(socket)
       << ", UUID=" << uuid;
    TLogNetwork(ss.str());

    TcpSocket peer;
    peer.SetSocket(socket);
    peer.Send(Protocol::LobbyRequestLogin(uuid).ToSring());
}
/*****************************************************************************/
void Lobby::ReadData(int socket, const std::string &data)
{
    ByteArray packet(data);
    std::uint32_t src_uuid = Protocol::GetSourceUuid(packet);
    std::uint32_t dest_uuid = Protocol::GetDestUuid(packet);

    if (mUsers.IsValid(src_uuid, socket))
    {
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
                        (*iter)->ExecuteRequest(packet);
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
            // Parse commands by the Lobby
            ExecuteRequest(packet);
        }
        else
        {
            std::stringstream ss;
            ss << "Packet destination must be the table or the lobby, nothing else; received UID: " << dest_uuid;
            TLogNetwork(ss.str());
        }
    }
    else
    {
        TLogNetwork("Invalid user!");
    }
}
/*****************************************************************************/
void Lobby::ExecuteRequest(const ByteArray &packet)
{
    mQueue.Push(packet); // Add packet to the queue
    Protocol::GetInstance().Execute(this); // Actually decode the packet
}
/*****************************************************************************/
bool Lobby::DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data)
{
    (void) dest_uuid;
    bool ret = true;
    ByteStreamReader in(data);

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

            Event event(Event::cIncPlayer);
            mSubject.Notify(event);
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

    return ret;
}
/*****************************************************************************/
ByteArray Lobby::GetPacket()
{
    ByteArray data;
    if (!mQueue.TryPop(data))
    {
        TLogError("Lobby: work item called without any data in the queue!");
    }
    return data;
}
/*****************************************************************************/
void Lobby::ClientClosed(int socket)
{
    std::stringstream ss;
    std::uint32_t uuid = mUsers.GetUuid(socket);
    std::uint32_t tableId = mUsers.GetPlayerTable(uuid);

    ss << "Player " << mUsers.GetIdentity(uuid).nickname << " has quit the server. UUID: " << uuid << ", socket=" << socket;
    TLogNetwork(ss.str());

    // Remove the player from the server
    mUsers.RemoveUser(uuid);
    // Remove the player from the table
    RemovePlayerFromTable(uuid, tableId);

    Event event(Event::cDecPlayer);
    mSubject.Notify(event);
}
/*****************************************************************************/
void Lobby::RemovePlayerFromTable(std::uint32_t uuid, std::uint32_t tableId)
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
void Lobby::ServerTerminated(TcpServer::IEvent::CloseType type)
{
    (void) type;
    TLogError("Server terminated (internal error)");
}
/*****************************************************************************/
void Lobby::SendData(const ByteArray &block)
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

    // Send the data to a(all) peer(s)
    for (std::list<std::uint32_t>::iterator iter = peers.begin(); iter != peers.end(); ++iter)
    {
        TcpSocket peer;
        peer.SetSocket(mUsers.GetSocket(*iter));
        peer.Send(block.ToSring());
        std::this_thread::sleep_for(std::chrono::milliseconds(20U));
    }

    if (peers.size() > 0U)
    {
        // Send data to listeners
        Event event(Event::cProtocolData);
        event.mBlock = block;
        mSubject.Notify(event);
    }
}
/*****************************************************************************/
std::string Lobby::GetTableName(const std::uint32_t tableId)
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
void Lobby::Stop()
{
	// Stop work item thread
	Protocol::GetInstance().Stop();

    mUsers.CloseClients();
    mTcpServer.Stop();

    mBotsMutex.lock();
    // Close local bots
    for (std::list<Bot *>::iterator iter = mBots.begin(); iter != mBots.end(); ++iter)
    {
        (*iter)->Close();
    }
    mBotsMutex.unlock();
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
bool Lobby::AddBot(std::uint32_t tableToJoin, const Identity &ident, std::uint16_t delay, const std::string &scriptFile)
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
    mBots.push_back(bot);
    mBotsMutex.unlock();

    // Connect the bot to the server
    bot->ConnectToHost("127.0.0.1", mTcpPort);

    return true;
}
/*****************************************************************************/
std::uint32_t Lobby::GetNumberOfBots(std::uint32_t tableId)
{
    std::lock_guard<std::mutex> lock(mBotsMutex);
    std::uint32_t counter = 0U;
    std::list<std::uint32_t> bots;

    // Look for connected bots
    for (std::list<Bot *>::iterator iter = mBots.begin(); iter != mBots.end(); ++iter)
    {
        bots.push_back((*iter)->GetUuid());
    }

    // Look for connected bots that are playing
    for (std::list<std::uint32_t>::iterator iter = bots.begin(); iter != bots.end(); ++iter)
    {
        if (mUsers.GetPlayerTable(*iter) == tableId)
        {
            counter++;
        }
    }

    return counter;
}
/*****************************************************************************/
/**
 * @brief Lobby::RemoveBot
 *
 * Removes a bot that belongs to a table. Also specify a place (south, north ...)
 *
 * @param tableId
 * @param p
 * @return
 */
bool Lobby::RemoveBot(std::uint32_t uuid)
{
    std::lock_guard<std::mutex> lock(mBotsMutex);
    bool ret = false;

    for (std::list<Bot *>::iterator iter = mBots.begin(); iter != mBots.end(); ++iter)
    {
        if ((*iter)->GetUuid() == uuid)
        {
            // Gracefully close the bot from the server
            (*iter)->Close();
            // delete the object
            delete (*iter);
            // Remove it from the list
            mBots.erase(iter);
            ret = true;
        }
    }
    return ret;
}
/*****************************************************************************/
void Lobby::RegisterListener(Observer<Event> &i_event)
{
    mSubject.Attach(i_event);
}

//=============================================================================
// End of file Lobby.cpp
//=============================================================================
