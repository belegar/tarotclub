/*=============================================================================
 * TarotClub - Table.cpp
 *=============================================================================
 * The table manages the Tarot thread server
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
#include "Table.h"

/*****************************************************************************/
Table::Table()
    : mControllerListener(*this)
    , mTcpPort(DEFAULT_PORT)
    , mIdManager(2U, 20U)
    , mAutoStart(true)
{

}
/*****************************************************************************/
void Table::Update(const TcpServer::Signal &info)
{
    if (info.type == TcpServer::DATA_RECEIVED)
    {
        mController.ExecuteRequest(info.data);
    }
    else if (info.type == TcpServer::NEW_CONNECTION)
    {
        std::uint32_t uuid = mIdManager.TakeId();
        // Add the player to the list

        mUsersMutex.lock();
        mUsers[uuid] = info.socket;
        mUsersMutex.unlock();
        // send the information to the Tarot engine
        mController.ExecuteRequest(Protocol::BuildAddPlayer(uuid));
    }

    // FIXME: manage client disconnection
    // FIXME: if a player has quit during a game, replace it by a bot
    //SendChatMessage("The player " + engine.GetPlayer(p).GetIdentity().name + " has quit the game.");
    //SendPlayersList();
}
/*****************************************************************************/
void Table::CloseClients()
{
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
}
/*****************************************************************************/
void Table::SendToSocket(const ByteArray &packet)
{
    // Extract the UUID from the data packet received
    std::uint32_t uuid = packet.GetUint32(3U);

    std::map<std::uint32_t, std::int32_t>::iterator iter;
    TcpSocket peer;

    mUsersMutex.lock();

    // Send the data to a(all) peer(s)
    for (iter = mUsers.begin(); iter != mUsers.end(); ++iter)
    {
        if (uuid == Protocol::ALL_PLAYERS ||
            iter->first == uuid)
        {
            peer.SetSocket(iter->second);
            peer.Send(packet.ToSring());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20U));
    }
    mUsersMutex.unlock();
}
/*****************************************************************************/
void Table::LoadConfiguration(int port)
{
    serverConfig.Load();

    // Apply configuration
    for (int i = 0; i < 3; i++)
    {
        mBots[i].SetIdentity(serverConfig.GetOptions().bots[i]);
        mBots[i].SetTimeBeforeSend(serverConfig.GetOptions().timer);
    }

    mTcpPort = port;
}
/*****************************************************************************/
void Table::SaveConfiguration(const ServerOptions &opt)
{
    serverConfig.SetOptions(opt);
    serverConfig.Save();
}
/*****************************************************************************/
void Table::CreateGame(Game::Mode gameMode, int nbPlayers, const Game::Shuffle &shuffle)
{
    // TODO: add support for 3 and 5 players game
    if (nbPlayers != 4)
    {
        TLogError("3 or 5 players are not yet supported");
        return;
    }
    mController.ExecuteRequest(Protocol::BuildNewGame(gameMode, nbPlayers, shuffle));
}
/*****************************************************************************/
void Table::StartDeal()
{
    mController.ExecuteRequest(Protocol::BuildStartDeal());
}
/*****************************************************************************/
void Table::Initialize()
{
    if (!TcpSocket::Initialize())
    {
        TLogError("Cannot initialize TCP context");
    }

    mTcpServer.RegisterListener(*this);
    mTcpServer.Start(mTcpPort, 10U);
    mController.RegisterListener(mControllerListener);
    mController.Start();
    for (int i = 0; i < 3; i++)
    {
        mBots[i].Initialize();
    }
}
/*****************************************************************************/
void Table::Stop()
{
    CloseClients();
    mTcpServer.Stop();

    // Close local bots
    for (int i = 0; i < 3; i++)
    {
        mBots[i].Close();
    }

    mController.Stop();
}
/*****************************************************************************/
ServerOptions &Table::GetOptions()
{
    return serverConfig.GetOptions();
}
/*****************************************************************************/
void Table::ConnectBots()
{
    int i;

    std::this_thread::sleep_for(std::chrono::milliseconds(50U));
    for (i = 0; i < 3; i++)
    {
        mBots[i].ConnectToHost("127.0.0.1", GetOptions().port);
        std::this_thread::sleep_for(std::chrono::milliseconds(50U));
    }
}

//=============================================================================
// End of file Table.cpp
//=============================================================================
