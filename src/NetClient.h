/*=============================================================================
 * TarotClub - NetClient.h
 *=============================================================================
 * TCP/IP network management for a client
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

#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include <thread>
#include "ThreadQueue.h"
#include "TcpClient.h"
#include "Protocol.h"

/*****************************************************************************/
class NetClient
{
public:
    enum Command
    {
        START,
        EXIT
    };

    class IEvent
    {
    public:
        static const std::uint32_t ErrDisconnectedFromServer    = 6000U;
        static const std::uint32_t ErrCannotConnectToServer     = 6001U;

        virtual void NetSignal(std::uint32_t sig) = 0;
    };

    NetClient(const std::shared_ptr<Protocol::IWorkItem> &item, IEvent &event);

    void Initialize();
    void ConnectToHost(const std::string &hostName, std::uint16_t port);
    bool IsConnected();
    void Disconnect();
    void Close();
    void SendPacket(const ByteArray &packet);

private:
    Protocol::IWorkItem::Data mItem;
    IEvent &mEvent;

    ThreadQueue<Command> mQueue;
    bool        mConnected;
    std::string mHostName;
    std::uint16_t mTcpPort;
    TcpClient   mTcpClient;
    std::thread mThread;
    bool        mInitialized;

    static void EntryPoint(void *pthis);
    void Run();
};


#endif // NET_CLIENT_H

//=============================================================================
// End of file NetClient.h
//=============================================================================
