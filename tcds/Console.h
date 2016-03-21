/*=============================================================================
 * TarotClub - Console.h
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

#ifndef CONSOLE_H
#define CONSOLE_H

#include "TcpServer.h"
#include "Lobby.h"

class Console : public TcpServer::IEvent
{
public:
    Console();
    ~Console();

    void Manage(Lobby &i_lobby, std::uint16_t port);

private:
    static const std::uint8_t cPeerData     = 0U;
    static const std::uint8_t cExitCommand  = 1U;

    struct Packet
    {
        std::uint8_t type;
        ByteArray data;
    };

    TcpServer   mTcpServer;
    Peer     mPeer; // Only one client allowed
    bool    mExit;
    ThreadQueue<Packet> mQueue;

    // From TcpServer interface
    virtual void NewConnection(const Peer &peer);
    virtual void ReadData(const Peer &peer, const ByteArray &data);
    virtual void ClientClosed(const Peer &peer);
    virtual void ServerTerminated(CloseType type);

};

#endif // CONSOLE_H

//=============================================================================
// End of file Console.h
//=============================================================================
