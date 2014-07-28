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
    : mTcpPort(DEFAULT_TCP_PORT)
    , mController(*this)
    , mIdManager(2U, 20U)
    , mTcpServer(*this)
    , mCreated(false)
    , mIsFull(false)
{

}
/*****************************************************************************/
void Table::NewConnection(int socket)
{
    std::uint32_t uuid = mIdManager.TakeId();
    // Add the player to the list

    mUsersMutex.lock();
    mUsers[uuid] = socket;
    mUsersMutex.unlock();
    // send the information to the Tarot engine
    mController.ExecuteRequest(Protocol::SystemAddPlayer(uuid));
}
/*****************************************************************************/
void Table::ReadData(int socket, const std::string &data)
{
    (void)socket; // not used for now FIXME can be used later to test if the socket exists
    mController.ExecuteRequest(data);
}
/*****************************************************************************/
void Table::ClientClosed(int socket)
{
    (void)socket;
    // FIXME: manage client disconnection
    // FIXME: if a player has quit during a game, replace it by a bot
    //SendChatMessage("The player " + engine.GetPlayer(p).GetIdentity().name + " has quit the game.");
    //SendPlayersList();
}
/*****************************************************************************/
void Table::SendData(const ByteArray &block)
{
    // Extract the UUID from the data packet received
    std::uint32_t uuid = block.GetUint32(3U);

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
            peer.Send(block.ToSring());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20U));
    }
    mUsersMutex.unlock();
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
/**
 * @brief Table::SetBotParameters
 * @param ident Identity of each bot. Size must be 3
 * @param delay delay between players
 */
void Table::SetBotParameters(std::map<Place, Identity> &ident, std::uint16_t delay)
{
    if (ident.size() == 3U)
    {
        for (std::uint32_t i = 0U; i < 3U; i++)
        {
            Place place(i + 1U);
            if (ident.count(place) > 0U)
            {
                Identity bot = ident[place];
                mBots[i].SetIdentity(bot);
                mBots[i].SetTimeBeforeSend(delay);
            }
            else
            {
                TLogError("Bad bot parameters!");
            }
        }
    }
}
/*****************************************************************************/
void Table::SetTcpPort(std::uint16_t port)
{
    mTcpPort = port;
}
/*****************************************************************************/
void Table::CreateTable(std::uint8_t nbPlayers)
{
    // TODO: add support for 3 and 5 players game
    if (nbPlayers != 4U)
    {
        TLogError("3 or 5 players are not yet supported");
        return;
    }

    if (mCreated)
    {
        CloseClients();
    }
    mController.ExecuteRequest(Protocol::SystemCreateTable(nbPlayers));
    mCreated = true;
    mIsFull = false;
}
/*****************************************************************************/
void Table::Initialize()
{
    if (!TcpSocket::Initialize())
    {
        TLogError("Cannot initialize TCP context");
    }

    mTcpServer.Start(mTcpPort, 10U);
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
std::uint16_t Table::GetTcpPort()
{
    return mTcpPort;
}
/*****************************************************************************/
void Table::ConnectBots()
{
    int i;

    std::this_thread::sleep_for(std::chrono::milliseconds(50U));
    for (i = 0; i < 3; i++)
    {
        mBots[i].ConnectToHost("127.0.0.1", mTcpPort);
        std::this_thread::sleep_for(std::chrono::milliseconds(50U));
    }
}

//=============================================================================
// End of file Table.cpp
//=============================================================================
