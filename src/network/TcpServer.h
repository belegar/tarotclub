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

#include <thread>
#include <vector>
#include <mutex>
#include <map>
#include "Observer.h"
#include "ThreadQueue.h"
#include "WebSocket.h"
#include "TcpServerBase.h"

/*****************************************************************************/
/**
 * @brief The TcpServer class
 *
 * General purpose Tcp Server thread.
 * Events on the server socket are throwed through the IEvent interface that
 * must be implemented in the user side of this class. The event handler is
 * required in the constructor.
 */
class TcpServer
{
public:
    class IEvent
    {
    public:
        enum CloseType
        {
            WAIT_SOCK_FAILED,
            TIMEOUT,
            CLOSED
        };

        /**
         * @brief NewConnection
         * Called when a new TCP/IP connection has been created
         * @param socket
         */
        virtual void NewConnection(const Peer &peer) = 0;

        /**
         * @brief ReadData
         * Called when new data have been sent by the peer to the server
         * @param socket
         * @param data
         */
        virtual void ReadData(const Peer &peer, const ByteArray &data) = 0;

        /**
         * @brief ClientClosed
         * Called when a client has closed its connection
         * @param socket
         */
        virtual void ClientClosed(const Peer &peer) = 0;

        /**
         * @brief ServerTerminated
         * Called when the server is about to shutdown (mainly because of an internal problem)
         */
        virtual void ServerTerminated(CloseType type) = 0;
    };

    TcpServer(IEvent &handler);

    virtual ~TcpServer(void) { }

    /**
     * @brief Start the Tcp thread server, with an optional WebSocket port to listen at
     * @param tcpPort
     * @param maxConnections
     * @param localHostOnly
     * @param wsPort
     * @return
     */
    bool Start(std::int32_t maxConnections, bool localHostOnly, std::uint16_t tcpPort, std::uint16_t wsPort = 0U);
    void Stop();
    void Join();
    std::string GetPeerName(int s);

private:
    struct Conn : public Peer
    {
        static const std::uint8_t cStateClosed      = 0U;
        static const std::uint8_t cStateConnected   = 1U;
        static const std::uint8_t cStateDeleteLater = 2U;

        Conn()
            : Peer()
            , state(cStateClosed)
        {

        }

        Conn(std::int32_t s, bool ws)
            : Peer(s, ws)
        {

        }

        Conn(const Peer &peer)
            : Peer(peer)
        {

        }

        bool IsClosed() const { return state == cStateClosed; }
        bool IsConnected()
        {
            bool connected = false;
            if (IsValid())
            {
                if (isWebSocket)
                {
                    if (state == cStateConnected)
                    {
                        connected = true;
                    }
                }
                else
                {
                    connected = true;
                }
            }
            return connected;
        }

        inline bool operator ==(const std::int32_t &rhs)
        {
            return (socket == rhs);
        }

        inline Conn & operator =(const std::int32_t &rhs)
        {
           socket = rhs;
           return *this;
        }

        inline bool operator >(const std::int32_t &rhs)
        {
            return (socket > rhs);
        }

        inline bool operator <(const Conn &rhs)
        {
            return (socket < rhs.socket);
        }

        std::uint8_t state;
        ByteArray wsPayload;
    };

    TcpServerBase   mTcpServer;
    TcpServerBase   mWsServer;
    std::thread mThread;
    int  mMaxSd;
    fd_set mMasterSet;
    std::vector<Conn> mClients;
    std::mutex mMutex; // To protect mClients
    bool mInitialized;
    IEvent     &mEventHandler;

    // Pipes on Linux to properly close the socket and quit select()
    int mReceiveFd;
    int mSendFd;

    static void EntryPoint(void *pthis);
    void Run();
    void IncommingConnection(bool isWebSocket);
    void IncommingData(Conn &conn);
    void UpdateMaxSocket();
    void ManageWsData(Conn &conn, const ByteArray &data);
    void DeliverWsData(Conn &conn, const ByteArray &data);
    std::string WsOpcodeToString(std::uint8_t opcode);
    void UpdateClients();
};


#endif // TCP_SERVER_H

//=============================================================================
// End of file TcpServer.h
//=============================================================================
