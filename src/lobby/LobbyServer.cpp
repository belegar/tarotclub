/*=============================================================================
 * TarotClub - LobbyServerServer.h
 *=============================================================================
 * Tcp peers management for the LobbyServer
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

#include "LobbyServer.h"
#include "Log.h"
#include "Util.h"
#include <sstream>
#include <vector>

struct D {
    void operator()(Protocol::IWorkItem* p) const
    {
        (void) p;
        // No destructor for this single object.
        // It will be naturally deleted while exiting from main
    }
};

/*****************************************************************************/
LobbyServer::LobbyServer(Lobby &lobby)
    : mLobby(lobby)
    , mWorkItem(std::shared_ptr<Protocol::IWorkItem>(&lobby, D()))
    , mTcpPort(ServerConfig::DEFAULT_GAME_TCP_PORT)
    , mTcpServer(*this)
{

}
/*****************************************************************************/
LobbyServer::~LobbyServer()
{
    Stop();
}
/*****************************************************************************/
void LobbyServer::Initialize(const ServerOptions &opt)
{
    if (!mInitialized)
    {
        // Initialize the controller
        mLobby.Initialize(opt.name, opt.tables, this);

        // Initialize all the tables, starting with the TCP port indicated
        mTcpPort = opt.game_tcp_port;
        mTcpServer.Start(opt.game_tcp_port, opt.lobby_max_conn, opt.localHostOnly);
        mInitialized = true;
    }
}
/*****************************************************************************/
void LobbyServer::WaitForEnd()
{
    mTcpServer.Join();
}
/*****************************************************************************/
void LobbyServer::NewConnection(int socket)
{
    mMutex.lock();
    std::uint32_t uuid = mLobby.AddUser();
    mPeers[uuid] = socket;
    mMutex.unlock();

    std::stringstream ss;
    ss << "New connection: socket=" << socket
       << ", IP=" << mTcpServer.GetPeerName(socket)
       << ", UUID=" << uuid;
    TLogNetwork(ss.str());

    TcpSocket peer;
    peer.SetSocket(socket);
    peer.Send(Protocol::LobbyRequestLogin(uuid).ToSring());

    Event event(Event::cIncPlayer);
    mSubject.Notify(event);
}
/*****************************************************************************/

/**
 * @brief LobbyServer::ReadData
 *
 * This callback is executed within the Tcp context. To miximize bandwidth,
 * we immediately manage the packet analysis in a worker thread.
 *
 * @param socket
 * @param data
 */
void LobbyServer::ReadData(int socket, const ByteArray &data)
{
    if (IsValid(Protocol::GetSourceUuid(data), socket))
    {
        mWorkItem.data = data;
        Protocol::GetInstance().Execute(mWorkItem); // Actually decode the packet
    }
}
/*****************************************************************************/
void LobbyServer::ClientClosed(int socket)
{
    std::uint32_t uuid = GetUuid(socket);

    mLobby.RemoveUser(uuid);

    Event event(Event::cDecPlayer);
    mSubject.Notify(event);
}
/*****************************************************************************/
void LobbyServer::ServerTerminated(TcpServer::IEvent::CloseType type)
{
    (void) type;
    TLogError("Server terminated (internal error)");
}
/*****************************************************************************/
void LobbyServer::Send(const ByteArray &data, std::list<std::uint32_t> peers)
{
    // Send the data to a(all) peer(s)
    for (std::list<std::uint32_t>::iterator iter = peers.begin(); iter != peers.end(); ++iter)
    {
        std::uint32_t uuid = (*iter);
        mMutex.lock();
        if (mPeers.count(uuid) > 0)
        {
            TcpSocket peer;
            peer.SetSocket(mPeers[uuid]);
            peer.Send(data.ToSring());
        }
        mMutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(20U));
    }

    if (peers.size() > 0U)
    {
        // Send data to listeners
        Event event(Event::cDataSent);
        event.mBlock = data;
        mSubject.Notify(event);
    }
}
/*****************************************************************************/
void LobbyServer::Stop()
{
    mInitialized = false;
    CloseClients();
    mTcpServer.Stop();
    mLobby.RemoveAllUsers();
}
/*****************************************************************************/
void LobbyServer::CloseClients()
{
    mMutex.lock();

    for (std::map<std::uint32_t, std::int32_t>::iterator iter = mPeers.begin(); iter != mPeers.end(); ++iter)
    {
        TcpSocket peer;
        peer.SetSocket(iter->second);
        peer.Close();
    }
    mMutex.unlock();
}
/*****************************************************************************/
void LobbyServer::RegisterListener(Observer<Event> &i_event)
{
    mSubject.Attach(i_event);
}
/*****************************************************************************/
bool LobbyServer::IsValid(std::uint32_t uuid, int socket)
{
    mMutex.lock();
    bool valid = false;

    if (mPeers.count(uuid) > 0U)
    {
        if (mPeers[uuid] == socket)
        {
            valid = true;
        }
    }
    mMutex.unlock();
    return valid;
}
/*****************************************************************************/
std::uint32_t LobbyServer::GetUuid(std::int32_t socket)
{
    mMutex.lock();
    std::uint32_t uuid = Protocol::INVALID_UID;
    for (std::map<std::uint32_t, std::int32_t>::iterator iter = mPeers.begin(); iter != mPeers.end(); ++iter)
    {
        if (iter->second == socket)
        {
            uuid = iter->first;
        }
    }
    mMutex.unlock();
    return uuid;
}

//=============================================================================
// End of file LobbyServer.cpp
//=============================================================================
