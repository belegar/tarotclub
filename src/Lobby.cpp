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

    // Stop before killing clients in case of remaining work items to run
    Protocol::GetInstance().Stop();

    // Kill tables
    for (std::list<Controller *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
    {
        delete (*iter);
    }

    // FIXME: kill bots
}
/*****************************************************************************/
void Lobby::Initialize(const ServerOptions &opt)
{
    Protocol::GetInstance().Initialize();

    Tarot::Shuffle sh;
    sh.type = Tarot::Shuffle::RANDOM_DEAL;

    // Initialize all the tables, starting with the TCP port indicated
    mTcpPort = opt.table_tcp_port;
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
    std::stringstream ss;
    ss << "New connection: socket=" << socket << ", IP=" << mTcpServer.GetPeerName(socket) << std::endl;
    TLogNetwork(ss.str());

    std::uint32_t uuid = mUsers.NewStagingUser(socket);

    TcpSocket peer;
    peer.SetSocket(socket);
    peer.Send(Protocol::LobbyRequestLogin(uuid).ToSring());
}
/*****************************************************************************/
void Lobby::ReadData(int socket, const std::string &data)
{
    ByteArray packet(data);
    std::uint32_t uuid = Protocol::GetSourceUuid(packet);

    if (mUsers.IsValid(uuid, socket))
    {
        // Filter using the socket:
        //   if the player is playing in a table, forward the data to the controller
        //   Otherwise, parse it, and treat chat messages only
        std::uint32_t tableId = mUsers.GetPlayerTable(uuid);
        if (tableId != 0U)
        {
            // forward it to the suitable controller
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
            // Parse commands by the Lobby
            ExecuteRequest(packet);
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
    case Protocol::CLIENT_MESSAGE:
    {
        std::string message;
        in >> message;

        message = mUsers.GetIdentity(src_uuid).name + "> " + message;
        SendData(Protocol::TableChatMessage(message));
        break;
    }

    case Protocol::CLIENT_REPLY_LOGIN:
    {
        Identity ident;
        in >> ident;

        mUsers.AccessGranted(src_uuid, ident);

        std::string message = "The player " + ident.name + " has joined the game.";
        SendData(Protocol::TableChatMessage(message));
        break;
    }

    case Protocol::CLIENT_JOIN_TABLE:
    {
        std::uint32_t tableId;
        in >> tableId;
        // Forward it to the table controller
        for (std::list<Controller *>::iterator iter = mTables.begin(); iter != mTables.end(); ++iter)
        {
            if ((*iter)->GetId() == tableId)
            {
                (*iter)->ExecuteRequest(Protocol::LobbyAddPlayer(src_uuid, mUsers.GetIdentity(src_uuid)));
            }
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
    mUsers.RemoveUser(uuid);

    // Fixme: remove the player from the table
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
    mUsers.SetPlayingTable(uuid, tableId);
}
/*****************************************************************************/
void Lobby::SendData(const ByteArray &block)
{
    std::uint32_t src_uuid = Protocol::GetSourceUuid(block);

    std::list<std::uint32_t> peers; // list of users to send the data

    // If the player is connected to a table, send data to the table only
    std::uint32_t tableId = mUsers.GetPlayerTable(src_uuid);
    if (tableId != Controller::NO_TABLE)
    {
        peers = mUsers.GetUsersOfTable(tableId);
    }



    // Otherwise, send the data to the lobby users only

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
void Lobby::CloseClients()
{
    /*
    std::map<std::uint32_t, std::int32_t>::iterator iter;
    TcpSocket peer;

    mUsersMutex.lock();

    // Send the data to a(all) peer(s)
    for (iter = mUsers.begin(); iter != mUsers.end(); ++iter)
    {
        peer.SetSocket(iter->second);
        peer.Close();
    }

    mUsers.clear();
    mUsersMutex.unlock();
    */
}
/*****************************************************************************/
void Lobby::Stop()
{
    /*
    CloseClients();
    mTcpServer.Stop();

    // Close local bots
    for (std::map<Place, Bot *>::iterator iter = mBots.begin(); iter != mBots.end(); ++iter)
    {
        (iter->second)->Close();
    }
    */
}
/*****************************************************************************/
void Lobby::RestApiRequest(int socket, const std::string &data)
{
    // This slot is called when the client sent data to the server. The
    // server looks if it was a get request and sends a very simple ASCII

    /*
    // Remove trailing \n
    std::string command = data;
    command.pop_back();

    std::vector<std::string> tokens = Util::Split(command, ":");
    std::stringstream ss;

    if (tokens[0] == "GET")
    {
        if (tokens[1] == "INFOS")
        {
            std::vector<std::string> list;

            for (int i = 0; i < SERVER_MAX_SALOONS; i++)
            {
                list.push_back(saloons[i].name);
            }

            ss << "SALOON:";
            ss << Util::Join(list, ",");
            ss << "\n";
        }
        else if (tokens[1] == "TABLES")
        {
            std::vector<std::string> list;

            for (int i = 0; i < SERVER_MAX_SALOONS; i++)
            {
                if (saloons[i].name == tokens[2])
                {
                    for (int j = 0; j < SERVER_MAX_TABLES; j++)
                    {
                        list.push_back(saloons[i].tables[j].name);
                    }
                }
            }

            ss << "TABLES:";
            ss << Util::Join(list, ",");
            ss << "\n";
        }
        else if (tokens[1] == "PORT")
        {
            std::vector<std::string> param = Util::Split(tokens[2], ",");

            for (int i = 0; i < SERVER_MAX_SALOONS; i++)
            {
                if (saloons[i].name == param[0])
                {
                    for (int j = 0; j < SERVER_MAX_TABLES; j++)
                    {
                        if (saloons[i].tables[j].name == param[1])
                        {
                            ss << "PORT:";
                            ss << saloons[i].tables[j].table.GetTcpPort();
                            ss << "\n";
                        }
                    }
                }
            }
        }
    }

    // If there is something to reply, do it
    if (ss.str().size() > 0)
    {
        sock.Send(ss.str());
    }
    else
    {
        // Or reply an error
        sock.Send("ERROR");
    }
    */
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

// FIXME: how to identity the bot (id? identity? name?)
bool Lobby::RemoveBot(Place p)
{
    std::lock_guard<std::mutex> lock(mBotsMutex);
    bool ret = false;

    /*

    std::map<Place, Bot *>::iterator iter = mBots.find(p);
    if (iter != mBots.end())
    {
        // Gracefully close the bot from the server
        iter->second->Close();
        // delete the object
        delete iter->second;
        // Remove it from the list
        mBots.erase(iter);
        ret = true;
    }
    */
    return ret;
}

//=============================================================================
// End of file Lobby.cpp
//=============================================================================
