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
#include "Defines.h"
#include <sstream>
#include <vector>


const std::string Lobby::LOBBY_VERSION_STRING = std::string("TarotClub ") + std::string(VERSION_STRING);

/*****************************************************************************/
Lobby::Lobby()
    : mTcpPort(ServerConfig::DEFAULT_LOBBY_TCP_PORT)
    , mTcpServer(*this)
{

}
/*****************************************************************************/
Lobby::~Lobby()
{
    Stop();

    // Kill tables
    for (std::list<Controller *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
    {
        delete (*iter);
    }

    // Kill bots
    for (std::list<Bot *>::iterator iter = mBots.begin(); iter != mBots.end(); ++iter)
    {
        delete (*iter);
    }
}
/*****************************************************************************/
void Lobby::Initialize(const ServerOptions &opt)
{
    Protocol::GetInstance().Initialize();

    Tarot::Shuffle sh;
    sh.type = Tarot::Shuffle::RANDOM_DEAL;

    // Initialize all the tables, starting with the TCP port indicated
    mTcpPort = opt.lobby_tcp_port;
    for (std::uint32_t i = 0U; i < opt.tables.size(); i++)
    {
        std::uint32_t id = i + 1U; // Id zero is not valid (means "no table")
        std::cout << "Creating table " << opt.tables[i] << ": id=" << id << std::endl;
        Controller *table = new Controller(*this);
        table->SetId(id);
        table->SetName(opt.tables[i]);
        table->Initialize(); // Start all threads and TCP sockets
        table->ExecuteRequest(Protocol::LobbyCreateTable(4U));
        mTables.push_back(table);
    }

    // Lobby TCP server
    mTcpServer.Start(opt.lobby_tcp_port, opt.lobby_max_conn);
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
        if (dest_uuid == Protocol::TABLE_UID)
        {
            // gets the table of the sender
            std::uint32_t tableId = mUsers.GetPlayerTable(src_uuid);
            if (tableId != Protocol::NO_TABLE)
            {
                // forward it to the suitable table controller
                for (std::list<Controller *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
                {
                    if ((*iter)->GetId() == tableId)
                    {
                        (*iter)->ExecuteRequest(packet);
                    }
                }
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
    case Protocol::CLIENT_LOBBY_MESSAGE:
    {
        std::string message;
        in >> message;

        message = mUsers.GetIdentity(src_uuid).name + "> " + message;
        SendData(Protocol::LobbyChatMessage(message), 0U);
        break;
    }

    case Protocol::CLIENT_REPLY_LOGIN:
    {
        Identity ident;
        in >> ident;

        if (mUsers.AccessGranted(src_uuid, ident))
        {
            SendData(Protocol::LobbyLoginResult(true, src_uuid), 0U);
            std::string message = "The player " + ident.name + " has joined the server.";
            TLogNetwork(message);
            SendData(Protocol::LobbyChatMessage(message), 0U);
            SendData(Protocol::LobbyPlayersList(mUsers.GetLobbyUserNames()), 0U);
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
            // Forward it to the table controller
            for (std::list<Controller *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
            {
                if ((*iter)->GetId() == tableId)
                {
                    (*iter)->ExecuteRequest(Protocol::LobbyAddPlayer(src_uuid, mUsers.GetIdentity(src_uuid)));
                }
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

    ss << "Player " << mUsers.GetIdentity(uuid).name << " has quit the server. UUID: " << uuid << ", socket=" << socket;
    TLogNetwork(ss.str());

    // Remove the player from the server
    mUsers.RemoveUser(uuid);
    // Remove the player from the table
    RemovePlayerFromTable(uuid, mUsers.GetPlayerTable(uuid));
}
/*****************************************************************************/
void Lobby::RemovePlayerFromTable(std::uint32_t uuid, std::uint32_t tableId)
{
    // Forward it to the table controller
    for (std::list<Controller *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
    {
        if ((*iter)->GetId() == tableId)
        {
            (*iter)->ExecuteRequest(Protocol::LobbyRemovePlayer(uuid));
        }
    }
}
/*****************************************************************************/
void Lobby::ServerTerminated(TcpServer::IEvent::CloseType type)
{
    (void) type;
    // FIXME: log an error
}
/*****************************************************************************/
void Lobby::AcceptPlayer(std::uint32_t uuid, std::uint32_t tableId)
{
    std::stringstream ss;
    ss << "Player " << mUsers.GetIdentity(uuid).name << " is entering on table: " << GetTableName(tableId);
    TLogNetwork(ss.str());
    mUsers.SetPlayingTable(uuid, tableId);
    SendData(Protocol::LobbyChatMessage(ss.str()), 0U);
}
/*****************************************************************************/
void Lobby::RemovePlayer(std::uint32_t uuid, std::uint32_t tableId)
{
    // Only send message and event if the player is still connected
    if (mUsers.IsHere(uuid))
    {
        std::stringstream ss;
        ss << "Player " << mUsers.GetIdentity(uuid).name << " is leaving table: " << GetTableName(tableId);
        TLogNetwork(ss.str());
        mUsers.SetPlayingTable(uuid, 0U);
        SendData(Protocol::LobbyChatMessage(ss.str()), 0U);
        SendData(Protocol::TableQuitEvent(uuid, tableId), 0U);
    }
}
/*****************************************************************************/
void Lobby::SendData(const ByteArray &block, std::uint32_t tableId)
{
    std::uint32_t dest_uuid = Protocol::GetDestUuid(block);

    std::list<std::uint32_t> peers; // list of users to send the data

    if (dest_uuid == Protocol::ALL_TABLE)
    {
        if (tableId != 0U)
        {
            // If the player is connected to a table, send data to the table players only
            peers = mUsers.GetUsersOfTable(tableId);
        }
        else
        {
            TLogError("Cannot send the packet to table ID zero! (reserved)");
        }
    }
    else if (dest_uuid == Protocol::ALL_LOBBY)
    {
        // Otherwise, send the data to the players connected in the lobby
        peers = mUsers.GetLobbyUsers();
    }
    else
    {
        // Only send the data to one connected client
        peers.push_back(dest_uuid);
    }

    // Debug: peers list cannot be null, or it is very strange ...
    if (peers.size() == 0)
    {
        TLogError("Peers list cannot be null");
    }

    // Send the data to a(all) peer(s)
    for (std::list<std::uint32_t>::iterator iter = peers.begin(); iter != peers.end(); ++iter)
    {
        TcpSocket peer;
        peer.SetSocket(mUsers.GetSocket(*iter));
        peer.Send(block.ToSring());
        std::this_thread::sleep_for(std::chrono::milliseconds(20U));
    }
}
/*****************************************************************************/
std::string Lobby::GetTableName(const std::uint32_t tableId)
{
    std::string name = "errot_table_not_found";
    // Forward it to the table controller
    for (std::list<Controller *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
    {
        if ((*iter)->GetId() == tableId)
        {
            name = (*iter)->GetName();
        }
    }
    return name;
}
/*****************************************************************************/
void Lobby::Stop()
{
	// Stop work item thread
	Protocol::GetInstance().Stop();

    mUsers.CloseClients();
    mTcpServer.Stop();

    // Close local bots
    for (std::list<Bot *>::iterator iter = mBots.begin(); iter != mBots.end(); ++iter)
    {
        (*iter)->Close();
    }
}
/*****************************************************************************/
std::string Lobby::ParseUri(const std::string &uri)
{
    std::string reply;
    if (uri == "/tables")
    {
        for (std::list<Controller *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
        {
            if (iter != mTables.begin())
            {
                reply += ",";
            }
            std::stringstream ss;
            ss << (*iter)->GetName() << "(" << (*iter)->GetId() << ")";
            reply += ss.str();
        }
    }
    else if (uri == "/players")
    {
        std::list<std::uint32_t> users = mUsers.GetLobbyUsers();
        for (std::list<std::uint32_t>::iterator iter = users.begin(); iter != users.end(); ++iter)
        {
            Identity ident = mUsers.GetIdentity(*iter);

            if (iter != users.begin())
            {
                reply += ",";
            }
            std::stringstream ss;
            ss << ident.name << "(" << *iter << ")";
            reply += ss.str();
        }
    }
    else if (uri == "/version")
    {
        reply = LOBBY_VERSION_STRING;
    }
    TLogNetwork("Received HTTP request: " + uri);

    return reply;
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
bool Lobby::AddBot(std::uint32_t tableToJoin, const Identity &ident, std::uint16_t delay)
{
    std::lock_guard<std::mutex> lock(mBotsMutex);

    // Place is free (not found), we dynamically add our bot here
    Bot *bot = new Bot();
    // Add it to the list (save the pointer to the allocated object)
    mBots.push_back(bot);
    // Initialize the bot
    bot->SetIdentity(ident);
    bot->SetTimeBeforeSend(delay);
    bot->SetTableToJoin(tableToJoin);
    bot->Initialize();
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

//=============================================================================
// End of file Lobby.cpp
//=============================================================================
