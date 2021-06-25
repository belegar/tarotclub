/*=============================================================================
 * TarotClub - Server.cpp
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

#include <cctype>
#include <iostream>
#include <string>
#include <sstream>
#include "Util.h"
#include "Server.h"
#include "System.h"
#include "Base64Util.h"

tcp::TcpServer::IEvent::~IEvent(){

}

/*****************************************************************************/
Server::Server(net::IEvent &listener)
    : mNetListener(listener)
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

    std::uint32_t uuid = mNetListener.AddUser(out);
    session.peer = conn.peer;
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

    if (uuid > Protocol::INVALID_UID)
    {
        Protocol &proto = mPeers[uuid].proto;

        if (mPeers[uuid].isPending)
        {
            TLogNetwork("Pending peer");

            proto.Add(conn.payload);
            Protocol::Header h;
            if (proto.Parse(data, h))
            {
                // La donnée transférée est la suivante
                // identifiant web : "passPhrase" chiffré avec la clé partagée
                // le tout en ascii, hex pour la partie chiffrée
                std::vector<std::string> sep = Util::Split(data, ":");
                if (sep.size() == 2)
                {
                    std::scoped_lock<std::mutex> lock(mMutex);
                    int64_t id = Util::FromString<int64_t>(sep[0]);
                    // on recherche l'ID dans la liste des clients
                    if (mAllowedClients.count(id) > 0)
                    {
                        // On déchiffre la deuxième partie du payload avec la clé GEK
                        mPeers[uuid].proto.SetSecurty(mAllowedClients[id].gek);
                        std::string passPhrase = sep[1]; // ici elle est encore cryptée
                        mPeers[uuid].proto.DecryptPayload(passPhrase, mAllowedClients[id].passPhrase.size());

                        if (passPhrase == mAllowedClients[id].passPhrase)
                        {
                            mPeers[uuid].isPending = false;
                        }
                        else
                        {
                            TLogNetwork("[SERVER] Bad pass phrase, expected: " + mAllowedClients[id].passPhrase + " decoded: " + passPhrase);
                        }
                    }
                    else
                    {
                        TLogNetwork("[SERVER] Unknown client");
                    }
                }
                else
                {
                    TLogNetwork("[SERVER] Bad frame size");
                }
            }
            else
            {
                TLogNetwork("[SERVER] Parse failure");
            }
        }
        else
        {
            proto.Add(conn.payload);
            Protocol::Header h;
            while (proto.Parse(data, h))
            {
                std::vector<Reply> out;

                //std::cout << "Found one packet with data: " << data << std::endl;
                mNetListener.Deliver(h.src_uid, h.dst_uid, data, out);
                Send(out);
            }
        }
    }
}
/*****************************************************************************/
void Server::ClientClosed(const tcp::Conn &conn)
{
    std::vector<Reply> out;
    std::uint32_t uuid = GetUuid(conn.peer);

    mNetListener.RemoveUser(uuid, out);
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
void Server::AddClient(int64_t id, const std::string &gek, const std::string &passPhrase)
{
    std::scoped_lock<std::mutex> lock(mMutex);
    Security sec;
    sec.id = id;
    sec.gek = gek;
    sec.passPhrase = passPhrase;
    mAllowedClients[id] = sec;
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
