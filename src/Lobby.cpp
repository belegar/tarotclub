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


const std::string Lobby::LOBBY_VERSION_STRING = std::string("TarotClub Lobby v") + std::string("2");

/*****************************************************************************/
Lobby::Lobby(LobbyController *controller)
    : mLobbyController(controller)
    , mWorkItem(controller)
    , mTcpPort(ServerConfig::DEFAULT_GAME_TCP_PORT)
    , mTcpServer(*this)
{

}
/*****************************************************************************/
Lobby::~Lobby()
{
    Stop();
}
/*****************************************************************************/
void Lobby::Initialize(const ServerOptions &opt)
{
    if (!mInitialized)
    {
        // Initialize the controller
        mLobbyController->Initialize(opt.name, opt.tables, this);

        // Initialize all the tables, starting with the TCP port indicated
        mTcpPort = opt.game_tcp_port;
        mTcpServer.Start(opt.game_tcp_port, opt.lobby_max_conn, opt.localHostOnly);
        mInitialized = true;
    }
}
/*****************************************************************************/
void Lobby::WaitForEnd()
{
    mTcpServer.Join();
}
/*****************************************************************************/
void Lobby::NewConnection(int socket)
{
    mMutex.lock();
    std::uint32_t uuid = mLobbyController->AddUser();
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
 * @brief Lobby::ReadData
 *
 * This callback is executed within the Tcp context. To miximize bandwidth,
 * we immediately manage the packet analysis in a worker thread.
 *
 * @param socket
 * @param data
 */
void Lobby::ReadData(int socket, const ByteArray &data)
{
    if (IsValid(Protocol::GetSourceUuid(data), socket))
    {
        mWorkItem.data = data;
        Protocol::GetInstance().Execute(mWorkItem); // Actually decode the packet
    }
}
/*****************************************************************************/
void Lobby::ClientClosed(int socket)
{
    std::stringstream ss;
    std::uint32_t uuid = GetUuid(socket);

    mLobbyController->RemoveUser(uuid);

    Event event(Event::cDecPlayer);
    mSubject.Notify(event);
}
/*****************************************************************************/
void Lobby::ServerTerminated(TcpServer::IEvent::CloseType type)
{
    (void) type;
    TLogError("Server terminated (internal error)");
}
/*****************************************************************************/
void Lobby::Send(const ByteArray &data, std::list<std::uint32_t> peers)
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
void Lobby::Stop()
{
    mInitialized = false;
    CloseClients();
    mTcpServer.Stop();
    mLobbyController->RemoveAllUsers();
}
/*****************************************************************************/
void Lobby::CloseClients()
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
void Lobby::RegisterListener(Observer<Event> &i_event)
{
    mSubject.Attach(i_event);
}
/*****************************************************************************/
bool Lobby::IsValid(std::uint32_t uuid, int socket)
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
std::uint32_t Lobby::GetUuid(std::int32_t socket)
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
// End of file Lobby.cpp
//=============================================================================
