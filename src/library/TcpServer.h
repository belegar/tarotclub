/*=============================================================================
 * TarotClub - TcpServer.h
 *=============================================================================
 * TCP socket wrapper class, server side
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

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <string>
#include <thread>
#include <vector>
#include "TcpSocket.h"
#include "Observer.h"

/*****************************************************************************/
class TcpServer : public TcpSocket
{
public:
    class IEvent
    {
    public:
        virtual void NewConnection(int socket) = 0;
        virtual void ReadData(const std::string &data) = 0;
        virtual void ClientClosed(int socket) = 0;
    };

    TcpServer(IEvent &handler);

    virtual ~TcpServer(void) { }

    bool Start(std::uint16_t port, std::int32_t maxConnections);
    void Stop();

private:
    std::thread mThread;
    int  mMaxSd;
    fd_set mMasterSet;
    std::vector<int> mClients;
    bool mInitialized;
    IEvent&     mEventHandler;

    static void EntryPoint(void *pthis);
    void Run();
    void IncommingConnection();
    bool IncommingData(int in_sock);
    void UpdateMaxSocket();
};


#endif // TCP_SERVER_H

//=============================================================================
// End of file TcpServer.h
//=============================================================================
