/*=============================================================================
 * TarotClub - Console.cpp
 *=============================================================================
 * TCP/IP console to manage the server using command line interface
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

#include "Console.h"
#include "Util.h"
#include "JsonValue.h"
#include "Version.h"

/*****************************************************************************/
Console::Console()
    : mTcpServer(*this)
    , mExit(false)
{

}
/*****************************************************************************/
Console::~Console()
{

}
/*****************************************************************************/
void Console::Manage(Lobby &i_lobby, std::uint16_t port)
{
    Packet packet;

    std::cout << "Starting management console on TCP port: " << port << std::endl;
    mTcpServer.Start(1U, true, port);

    while (!mExit)
    {
        mQueue.WaitAndPop(packet);

        if (packet.type == cPeerData)
        {
            // Decode command
            std::int32_t pos = packet.data.FindFirstOf('\n');
            if (pos >= 0)
            {
                // Remove trailing '\n'
                packet.data.Alloc(pos);
                if (packet.data == "exit")
                {
                    mExit = true;
                }
                else if (packet.data == "status")
                {
                    TcpSocket sock(mPeer);
                    JsonObject status;
                    status.AddValue("status", true);
                    status.AddValue("name", i_lobby.GetName());
                    status.AddValue("version", TCDS_VERSION);
                    status.AddValue("players", (std::int32_t)i_lobby.GetNumberOfPlayers());
                    sock.Send(status.ToString(0U));
                }
            }
        }
        else if (packet.type == cExitCommand)
        {
            TcpSocket sock(mPeer);
            sock.Send("Exiting server ...\n");
            mExit = true;
        }
    }

    // Close properly, wait for the thread to stop
    mTcpServer.Stop();
}
/*****************************************************************************/
void Console::NewConnection(const Peer &peer)
{
    // Save socket
    mPeer = peer;
}
/*****************************************************************************/
void Console::ReadData(const Peer &peer, const ByteArray &data)
{
    if (mPeer == peer)
    {
        Packet packet;
        packet.type = cPeerData;
        packet.data = data;
        mQueue.Push(packet);
    }
}
/*****************************************************************************/
void Console::ClientClosed(const Peer &peer)
{
    (void) peer;
    mPeer.socket = -1;
}
/*****************************************************************************/
void Console::ServerTerminated(TcpServer::IEvent::CloseType type)
{
    (void) type;
    mPeer.socket = -1;
}

//=============================================================================
// End of file Console.cpp
//=============================================================================

