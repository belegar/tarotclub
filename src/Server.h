/*=============================================================================
 * TarotClub - Server.h
 *=============================================================================
 * Management of raw network protocol
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

#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include "Value.h"
#include "Lobby.h"
#include "Protocol.h"
#include "JsonValue.h"
#include "BotManager.h"
#include "TcpServer.h"
#include "Observer.h"
#include <mutex>
#include <thread>
#include <chrono>

/*****************************************************************************/
/**
 * @brief The Tarot TCP/IP Server class
 *
 * This server does not know nothing about the tarot, it is only the network part.
 * The TCP/IP server class is a thread, so we execute everything on that context.
 *
 * Beware to protect any shared data with external access.
 *
 */
class Server : private tcp::TcpServer::IEvent
{
public:
    Server(net::IEvent &listener);

    void Start(const ServerOptions &opt);
    void Stop();
    void Send(const std::vector<Reply> &out);
    bool IsStarted() { return mTcpServer.IsStarted(); }
    void AddClient(int64_t id, const std::string &gek, const std::string &passPhrase);

private:
    // From TcpServer interface
    // ALl these callbacks are executed within the same thread
    virtual void NewConnection(const tcp::Conn &conn);
    virtual void ReadData(const tcp::Conn &conn);
    virtual void ClientClosed(const tcp::Conn &conn);
    virtual void ServerTerminated(tcp::TcpServer::IEvent::CloseType type);

    struct PeerSession {
        Protocol proto;
        tcp::Peer peer;
        bool isPending;
        PeerSession()
            : isPending(true)
        {

        }
    };

    struct Security {
        int64_t id;
        std::string gek;
        std::string passPhrase;
    };

    net::IEvent &mNetListener;
    tcp::TcpServer       mTcpServer;
    std::mutex  mMutex;
    std::map<std::uint32_t, PeerSession> mPeers; // uuid <--> GameSession
    std::map<std::int64_t, Security> mAllowedClients; // allowed peers on this server, key=playerId, value: GEK

    void CloseClients();
    std::uint32_t GetUuid(const tcp::Peer &peer);
    bool IsPending(const tcp::Peer &peer);
};


#endif // SERVER_H

//=============================================================================
// End of file Server.h
//=============================================================================
