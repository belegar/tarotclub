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
#include "ServerConfig.h"
#include "Observer.h"
#include "LobbyController.h"

/*****************************************************************************/
class Lobby : private TcpServer::IEvent, private LobbyController::IPacketNotifier
{

public:
    static const std::string LOBBY_VERSION_STRING;

    struct Event
    {
        static const std::uint32_t cIncPlayer    = 0U;
        static const std::uint32_t cDecPlayer    = 1U;
        static const std::uint32_t cDataSent = 2U;

        Event(std::uint32_t type)
            : mType(type)
         {}

        std::uint32_t mType;
        ByteArray mBlock;
    };

    Lobby(LobbyController *controller);
    ~Lobby();

    void Initialize(const ServerOptions &opt);
    void Stop();
    void WaitForEnd();

    // Lobby management
    void CloseClients();
    void RegisterListener(Observer<Event> &i_event);

private:

    // From TcpServer interface
    virtual void NewConnection(int socket);
    virtual void ReadData(int socket, const ByteArray &data);
    virtual void ClientClosed(int socket);
    virtual void ServerTerminated(CloseType type);

    // From LobbyController::IPacketNotifier
    virtual void Send(const ByteArray &data, std::list<std::uint32_t> peers);

    LobbyController *mLobbyController;
    Protocol::IWorkItem::Data mWorkItem;
    Subject<Event>  mSubject;
    int             mTcpPort;
    TcpServer       mTcpServer;
    bool            mInitialized;
    bool            mIsFull;

    // Maintain a list of connected peers
    std::map<std::uint32_t, std::int32_t> mPeers; // pair of uuid, socket
    std::mutex mMutex;

    void SendDataToPlayer(const ByteArray &data);
    std::uint32_t GetUuid(std::int32_t socket);
    bool IsValid(std::uint32_t uuid, int socket);
};

#endif // LOBBY_H

//=============================================================================
// End of file Lobby.h
//=============================================================================
