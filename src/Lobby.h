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
#include "Observer.h"

/*****************************************************************************/
class Lobby : public Protocol::IWorkItem, public TcpServer::IEvent, public Controller::IData
{

public:
    static const std::string LOBBY_VERSION_STRING;

    struct Event
    {
        static const std::uint32_t cIncPlayer    = 0U;
        static const std::uint32_t cDecPlayer    = 1U;
        static const std::uint32_t cProtocolData = 2U;

        Event(std::uint32_t type)
            : mType(type)
         {}

        std::uint32_t mType;
        ByteArray mBlock;
    };


    Lobby();
    ~Lobby();

    void Initialize(const ServerOptions &opt);
    void Stop();
    void WaitForEnd();

    // Users management
    std::uint32_t GetNumberOfPlayers();

    // Bots management
    bool AddBot(std::uint32_t tableToJoin, const Identity &ident, std::uint16_t delay, const std::string &scriptFile);
    std::uint32_t GetNumberOfBots(std::uint32_t tableId);
    bool RemoveBot(std::uint32_t uuid);
    void ChangeBotIdentity(std::uint32_t uuid, const Identity &identity);

    // Lobby management
    void CloseClients();
    void RegisterListener(Observer<Event> &i_event);
    std::string GetName() { return mName; }

    // Tables management
    std::uint32_t CreateTable(const std::string &tableName, bool adminMode = true, const Tarot::Game &game = Tarot::Game());
    bool DestroyTable(std::uint32_t id);

private:

    // From TcpServer interface
    virtual void NewConnection(int socket);
    virtual void ReadData(int socket, const std::string &data);
    virtual void ClientClosed(int socket);
    virtual void ServerTerminated(CloseType type);

    // Called from the table controllers and Lobby
    void SendData(const ByteArray &block);

    // From Protocol::WorkItem
    bool DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data);
    ByteArray GetPacket();

    void ExecuteRequest(const ByteArray &packet);
    std::string GetTableName(const std::uint32_t tableId);
	void RemovePlayerFromTable(std::uint32_t uuid, std::uint32_t tableId);

    Subject<Event>  mSubject;
    int             mTcpPort;
    TcpServer       mTcpServer;
    bool            mInitialized;
    bool            mIsFull;
    ThreadQueue<ByteArray> mQueue;      //!< Queue of network packets received

    std::vector<Controller *> mTables;
    UniqueId    mTableIds;
    std::mutex mTablesMutex;
    Users       mUsers;

    // Bots management
    std::list<Bot *> mBots;
    std::mutex mBotsMutex;
    std::string mName;

    void SendDataToPlayer(const ByteArray &data);
};

#endif // LOBBY_H

//=============================================================================
// End of file Lobby.h
//=============================================================================
