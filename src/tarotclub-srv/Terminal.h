/*=============================================================================
 * TarotClub - Terminal.h
 *=============================================================================
 * TCP/IP console terminal to manage the server using command line interface
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

#ifndef TERMINAL_H
#define TERMINAL_H

#include "TcpServer.h"
#include "Lobby.h"

class Terminal : public tcp::TcpServer::IEvent
{
public:
    Terminal();
    ~Terminal();

    void Manage(Lobby &i_lobby, std::uint16_t port);

private:
    static const std::uint8_t cPeerData     = 0U;
    static const std::uint8_t cExitCommand  = 1U;

    tcp::TcpServer   mTcpServer;
    tcp::Peer     mPeer; // Only one client allowed
    bool    mExit;
  //  ThreadQueue<Packet> mQueue;

    // From TcpServer interface
    void NewConnection(const tcp::Conn &conn);
    void ReadData(const tcp::Conn &conn);
    void ClientClosed(const tcp::Conn &conn);
    void ServerTerminated(tcp::TcpServer::IEvent::CloseType type);

};

#endif // TERMINAL_H

//=============================================================================
// End of file Terminal.h
//=============================================================================
