/*=============================================================================
 * TarotClub - LobbyServer.h
 *=============================================================================
 * Tcp peers management for the Lobby
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#ifndef LOBBY_SERVER_H
#define LOBBY_SERVER_H

#include "TcpServer.h"
#include "ServerConfig.h"
#include "Observer.h"
#include "Lobby.h"

/*****************************************************************************/
class LobbyServer : private TcpServer::IEvent, private Lobby::IPacketNotifier
{

public:

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

    LobbyServer(Lobby &lobby);
    ~LobbyServer();

    void Initialize(const ServerOptions &opt);
    void Stop();
    void WaitForEnd();

    // LobbyServer management
    void CloseClients();
    void RegisterListener(Observer<Event> &i_event);

private:

    // From TcpServer interface
    virtual void NewConnection(const Peer &peer);
    virtual void ReadData(const Peer &peer, const ByteArray &data);
    virtual void ClientClosed(const Peer &peer);
    virtual void ServerTerminated(CloseType type);

    // From Lobby::IPacketNotifier
    virtual void Send(const ByteArray &data, std::list<std::uint32_t> peers);

    Lobby &mLobby;
    Protocol::IWorkItem::Data mWorkItem;
    Subject<Event>  mSubject;
    int             mTcpPort;
    TcpServer       mTcpServer;
    bool            mInitialized;

    // Maintain a list of connected peers
    std::map<std::uint32_t, Peer> mPeers; // pair of uuid, Peer info
    std::mutex mMutex;

    void SendDataToPlayer(const ByteArray &data);
    std::uint32_t GetUuid(const Peer &peer);
    bool IsValid(std::uint32_t uuid, const Peer &peer);
};

#endif // LOBBY_SERVER_H

//=============================================================================
// End of file LobbyServer.h
//=============================================================================
