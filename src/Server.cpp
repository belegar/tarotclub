/*=============================================================================
 * TarotClub - Server.cpp
 *=============================================================================
 * Server, game modes, database and statistics management
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

#include <cctype>
#include <iostream>
#include <string>
#include <sstream>
#include "Util.h"
#include "Server.h"
#include "System.h"

tcp::TcpServer::IEvent::~IEvent(){

}

/*****************************************************************************/
Server::Server(net::IEvent &listener)
    : mListener(listener)
    , mTcpServer(*this)
{

}
/*****************************************************************************/
void Server::Start(const ServerOptions &opt)
{
    if (IsStarted())
    {
        mTcpServer.Stop();
    }
    mTcpServer.Start(opt.lobby_max_conn, opt.localHostOnly, opt.game_tcp_port, opt.websocket_tcp_port);
}
/*****************************************************************************/
void Server::NewConnection(const tcp::Conn &conn)
{
    PeerSession session;
    std::vector<Reply> out;

    std::uint32_t uuid = mListener.AddUser(out);
    session.peer = conn.peer;
    session.proto.SetSecurty("aPdSgVkYp3s6v9y"); // FIXME generate per-user key
    mPeers[uuid] = session;

    Send(out);
}
/*****************************************************************************/
/**
 * @brief Server::ReadData
 *
 * This callback is executed within the Tcp context. Here we are receiving data
 * from peers.
 */
void Server::ReadData(const tcp::Conn &conn)
{
    std::string data;
    std::uint32_t uuid = GetUuid(conn.peer);

    if (uuid != Protocol::INVALID_UID)
    {
        Protocol &proto = mPeers[uuid].proto;
        proto.Add(conn.payload);
        Protocol::Header h;
        while (proto.Parse(data, h))
        {
            std::vector<Reply> out;

            //std::cout << "Found one packet with data: " << data << std::endl;
            mListener.Deliver(h.src_uid, h.dst_uid, data, out);
            Send(out);
        }
    }
}
/*****************************************************************************/
void Server::ClientClosed(const tcp::Conn &conn)
{
    std::vector<Reply> out;
    std::uint32_t uuid = GetUuid(conn.peer);

    mListener.RemoveUser(uuid, out);
    Send(out);
}
/*****************************************************************************/
void Server::ServerTerminated(tcp::TcpServer::IEvent::CloseType type)
{
    (void) type;
    TLogError("Server terminated (internal error)");
}
/*****************************************************************************/
void Server::Stop()
{
    // Properly stop the thread
    mTcpServer.Stop();
    mTcpServer.Join();
}
/*****************************************************************************/
void Server::Send(const std::vector<Reply> &out)
{
    // Send all data
    for (std::uint32_t i = 0U; i < out.size(); i++)
    {
        std::string data = out[i].data.ToString();
        // To all indicated peers
        for (std::uint32_t j = 0U; j < out[i].dest.size(); j++)
        {
            std::uint32_t uuid = out[i].dest[j];
            if (mPeers.count(uuid) > 0)
            {
                tcp::TcpSocket::Send(mPeers[uuid].proto.Build(Protocol::LOBBY_UID, uuid, data), mPeers[uuid].peer);
            }
            else
            {
                TLogError("[Server] Cannot find peer");
            }
        }
    }
}
/*****************************************************************************/
void Server::CloseClients()
{
    for (std::map<std::uint32_t, PeerSession>::iterator iter = mPeers.begin(); iter != mPeers.end(); ++iter)
    {
        tcp::TcpSocket::Close(iter->second.peer);
    }
}
/*****************************************************************************/
std::uint32_t Server::GetUuid(const tcp::Peer &peer)
{
    std::uint32_t uuid = Protocol::INVALID_UID;
    for (std::map<std::uint32_t, PeerSession>::iterator iter = mPeers.begin(); iter != mPeers.end(); ++iter)
    {
        if (iter->second.peer == peer)
        {
            uuid = iter->first;
        }
    }
    return uuid;
}


//=============================================================================
// End of file Server.cpp
//=============================================================================
