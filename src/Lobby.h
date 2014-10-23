/*=============================================================================
 * TarotClub - Lobby.h
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

#ifndef LOBBY_H
#define LOBBY_H

#include "TcpServer.h"
#include "Controller.h"
#include "ServerConfig.h"
#include "Bot.h"
#include "Users.h"

/*****************************************************************************/
class Lobby : public Protocol::IWorkItem, public TcpServer::IEvent, public Controller::IEvent
{

public:
    Lobby();
    ~Lobby();

    void Initialize(const ServerOptions &opt);
    void Stop();
    void WaitForEnd();

    // HTTP / REST API
    std::string ParseUri(const std::string &uri);

    // Bots management
    bool AddBot(std::uint32_t tableToJoin, const Identity &ident, std::uint16_t delay);
    std::uint32_t GetNumberOfBots(std::uint32_t tableId);
    bool RemoveBot(std::uint32_t uuid);

    void CloseClients();
private:

    // From TcpServer interface
    virtual void NewConnection(int socket);
    virtual void ReadData(int socket, const std::string &data);
    virtual void ClientClosed(int socket);
    virtual void ServerTerminated(CloseType type);

    // From Controller
    virtual void AcceptPlayer(std::uint32_t uuid, std::uint32_t tableId);
    virtual void SendData(const ByteArray &block, std::uint32_t tableId);

    int             mTcpPort;
    TcpServer       mTcpServer;
    bool            mCreated;   ///< True if the table has been created
    bool            mIsFull;
    ThreadQueue<ByteArray> mQueue;      //!< Queue of network packets received

    std::list<Controller *> mTables;
    Users   mUsers;

    // Bots management
    std::list<Bot *> mBots;
    std::mutex mBotsMutex;

    std::uint32_t PlayerTable(std::uint32_t uuid);
    void ExecuteRequest(const ByteArray &packet);

    // From Protocol::WorkItem
    bool DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data);
    ByteArray GetPacket();

};

#endif // LOBBY_H

//=============================================================================
// End of file Lobby.h
//=============================================================================
