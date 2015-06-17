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
        virtual void NewConnection(int socket) = 0;

        /**
         * @brief ReadData
         * Called when new data have been sent by the peer to the server
         * @param socket
         * @param data
         */
        virtual void ReadData(int socket, const ByteArray &data) = 0;

        /**
         * @brief ClientClosed
         * Called when a client has closed its connection
         * @param socket
         */
        virtual void ClientClosed(int socket) = 0;

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
    class Peer
    {
    public:
        static const std::uint8_t cStateClosed = 0U;

        Peer()
            : mSocket(-1)
            , mState(0U)
            , mIsWebSocket(false)
        {

        }

        Peer(std::int32_t socket, bool ws)
            : mSocket(socket)
            , mState(0U)
            , mIsWebSocket(ws)
        {

        }

        inline bool operator ==(const std::int32_t &rhs)
        {
            return (mSocket == rhs);
        }

        inline Peer & operator =(const std::int32_t &rhs)
        {
           mSocket = rhs;
           return *this;
        }

        inline bool operator >(const std::int32_t &rhs)
        {
            return (mSocket > rhs);
        }

        inline bool operator <(const Peer &rhs)
        {
            return (mSocket < rhs.mSocket);
        }

        bool IsWebSocket() const { return mIsWebSocket; }

        std::int32_t GetSocket() const { return mSocket; }
    private:
        std::int32_t mSocket;
        std::uint8_t mState;
        bool mIsWebSocket;

    };

    TcpServerBase   mTcpServer;
    TcpServerBase   mWsServer;
    std::thread mThread;
    int  mMaxSd;
    fd_set mMasterSet;
    std::vector<Peer> mClients;
    std::mutex mMutex; // To protect mClients
    bool mInitialized;
    IEvent     &mEventHandler;

    // Pipes on Linux to properly close the socket and quit select()
    int mReceiveFd;
    int mSendFd;

    static void EntryPoint(void *pthis);
    void Run();
    void IncommingConnection(bool isWebSocket);
    bool IncommingData(const Peer &peer);
    void UpdateMaxSocket();
    void ManageWsData(const Peer &peer, const ByteArray &data);
};


#endif // TCP_SERVER_H

//=============================================================================
// End of file TcpServer.h
//=============================================================================
