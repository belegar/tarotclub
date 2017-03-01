/*=============================================================================
 * TarotClub - TcpServer.cpp
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

#include <algorithm>
#include <cstring>
#include "TcpServer.h"
#include "Log.h"
#include <iostream>
#include <sstream>

namespace tcp
{

/*****************************************************************************/
TcpServer::TcpServer(IEvent &handler)
    : mMaxSd(0)
    , mInitialized(false)
    , mEventHandler(handler)
    , mReceiveFd(-1)
    , mSendFd(-1)
{
    FD_ZERO(&mMasterSet);
}

/*****************************************************************************/
bool TcpServer::Start(std::int32_t maxConnections, bool localHostOnly, std::uint16_t tcpPort, std::uint16_t wsPort)
{
    mTcpServer.CreateServer(tcpPort, localHostOnly, maxConnections);
    if (wsPort > 0U)
    {
        mWsServer.CreateServer(wsPort, localHostOnly, maxConnections);
    }

    // Create the thread the first time only
    if (!mInitialized)
    {
        mThread = std::thread(TcpServer::EntryPoint, this);
        mInitialized = true;
    }

    return mInitialized;
}
/*****************************************************************************/
void TcpServer::Stop()
{
    if (mSendFd > 0)
    {
#ifdef USE_WINDOWS_OS
        _write(mSendFd, "1", 1);
#else
		write(mSendFd, "1", 1);
#endif
    }
    mTcpServer.Close();
    mWsServer.Close();
    Join();
}
/*****************************************************************************/
void TcpServer::Join()
{
    if (mInitialized)
    {
        mThread.join();
        mInitialized = false;
    }
}
/*****************************************************************************/
// s must be a valid socket, otherwise the returning string is null
std::string TcpServer::GetPeerName(int s)
{
    std::stringstream ss;
    socklen_t len;
    struct sockaddr_storage addr;
    std::string ipstr;
    int port = 0;

    len = sizeof(addr);
    int ret = getpeername(s, (struct sockaddr*)&addr, &len);
    if (ret == 0)
    {
		ipstr = TcpSocket::ToString((struct sockaddr*)&addr); // point to an internal static buffer
        ss << ipstr << ":" << port;
    }
    else
    {
        ss << "Cannot get peer name!";
    }

    return ss.str();
}
/*****************************************************************************/
void TcpServer::Run()
{
    bool end_server = false;
    //   struct timeval timeout;
    fd_set working_set;

    /*************************************************************/
    /* Initialize the master fd_set                              */
    /*************************************************************/
    FD_ZERO(&mMasterSet);
    UpdateMaxSocket();
    FD_SET(mTcpServer.GetSocket(), &mMasterSet);
    if (mWsServer.IsValid())
    {
        FD_SET(mWsServer.GetSocket(), &mMasterSet);
    }

#ifdef USE_UNIX_OS
    /*************************************************************/
    /* Use a pipe on Unix to gracefully quit the select(),       */
    /* close the socket and finally exit the thread              */
    /*************************************************************/
    int pipefd[2];
    if (pipe(pipefd) != 0)
    {
        std::cout << "Pipe creation error, exiting ..." << std::endl;
        return;
    }

    mReceiveFd = pipefd[0];
    mSendFd = pipefd[1];
    fcntl(mSendFd, F_SETFL, O_NONBLOCK);
    mMaxSd = (mMaxSd > mReceiveFd) ? mMaxSd : mReceiveFd;
    FD_SET(mReceiveFd, &mMasterSet);
#endif

    /*************************************************************/
    /* Initialize the timeval struct to N minutes.  If no        */
    /* activity after N minutes this program will end.           */
    /* 0 means unlimited (== no timeout)                         */
    /*************************************************************/
    //   timeout.tv_sec  = 0; //10 * 60;
    //   timeout.tv_usec = 0;

    /*************************************************************/
    /* Loop waiting for incoming connects or for incoming data   */
    /* on any of the connected sockets.                          */
    /*************************************************************/
    do
    {
        /**********************************************************/
        /* Copy the master fd_set over to the working fd_set.     */
        /**********************************************************/
        std::memcpy(&working_set, &mMasterSet, sizeof(mMasterSet));

        /**********************************************************/
        /* Call select() and wait N minutes for it to complete.   */
        /**********************************************************/
        //    printf("Waiting on select()...\n");
        int rc = select(mMaxSd + 1, &working_set, NULL, NULL, NULL); // &timeout);

        if (rc < 0)
        {
            /**********************************************************/
            /* The select call failed.                                */
            /**********************************************************/
            if (!mTcpServer.AnalyzeSocketError("select()"))
            {
                end_server = true;
                mEventHandler.ServerTerminated(IEvent::WAIT_SOCK_FAILED);
            }
        }
        else if (rc == 0)
        {
            /**********************************************************/
            /* The time out expired.                                  */
            /**********************************************************/
            if (!mTcpServer.AnalyzeSocketError("select()"))
            {
                end_server = true;
                mEventHandler.ServerTerminated(IEvent::TIMEOUT);
            }
        }
        else
        {
            /**********************************************************/
            /* One or more descriptors are readable.  Need to         */
            /* determine which ones they are.                         */
            /**********************************************************/
            for (int i = 0; i < rc; i++)
            {
                if (FD_ISSET(mReceiveFd, &working_set))
                {
                    end_server = true;
                    mEventHandler.ServerTerminated(IEvent::CLOSED);
                    break;
                }
                else if (FD_ISSET(mTcpServer.GetSocket(), &working_set))
                {
                    /****************************************************/
                    /* This is the listening socket                     */
                    /****************************************************/
                    mMutex.lock();
                    IncommingConnection(false);
                    mMutex.unlock();
                }
                else if (FD_ISSET(mWsServer.GetSocket(), &working_set))
                {
                    /****************************************************/
                    /* This is the listening socket                     */
                    /****************************************************/
                    mMutex.lock();
                    IncommingConnection(true);
                    mMutex.unlock();
                }
                else
                {
                    mMutex.lock();

                    // Scan for already connected clients
                    for (size_t j = 0; j < mClients.size(); j++)
                    {
                        if (FD_ISSET(mClients[j].peer.socket, &working_set))
                        {
                            /****************************************************/
                            /* This is not the listening socket, therefore an   */
                            /* existing connection must be readable             */
                            /****************************************************/
                            IncommingData(mClients[j]);
                        }
                    }

                    UpdateClients(); // refresh status, manage proper closing if necessary
                    mMutex.unlock();
                }
            }
        }
    }
    while (end_server == false);

    /*************************************************************/
    /* Cleanup all of the sockets that are open                  */
    /*************************************************************/
    mMutex.lock();
    for (size_t i = 0; i < mClients.size(); i++)
    {
        if (FD_ISSET(mClients[i].peer.socket, &mMasterSet))
        {
            TcpSocket::Close(mClients[i].peer);
        }
    }
    mMutex.unlock();
}
/*****************************************************************************/
void TcpServer::EntryPoint(void *pthis)
{
    TcpServer *pt = static_cast<TcpServer *>(pthis);
    pt->Run();
}
/*****************************************************************************/
void TcpServer::IncommingConnection(bool isWebSocket)
{
    int new_sd;
    bool webSocket = false;

    //    printf("  Listening socket is readable\n");
    /*************************************************/
    /* Accept all incoming connections that are      */
    /* queued up on the listening socket before we   */
    /* loop back and call select again.              */
    /*************************************************/
    do
    {
        /**********************************************/
        /* Accept each incoming connection.  If       */
        /* accept fails with EWOULDBLOCK, then we     */
        /* have accepted all of them.  Any other      */
        /* failure on accept will cause us to end the */
        /* server.                                    */
        /**********************************************/
        if (isWebSocket)
        {
            new_sd = mWsServer.Accept();
            webSocket = true;
        }
        else
        {
            new_sd = mTcpServer.Accept();
        }

        if (new_sd >= 0)
        {
            Conn newPeer(new_sd, webSocket);
            // Save peers descriptor
            mClients.push_back(newPeer);

            /**********************************************/
            /* Add the new incoming connection to the     */
            /* master read set                            */
            /**********************************************/
            //     printf("  New incoming connection - %d\n", new_sd);
            FD_SET(new_sd, &mMasterSet);

            // Update the maximum socket file identifier
            UpdateMaxSocket();

            if (!webSocket)
            {
                // Signal a new client only if not a web socket (need a handshake before considering it is connected)
                mEventHandler.NewConnection(newPeer);
            }
        }

        /**********************************************/
        /* Loop back up and accept another incoming   */
        /* connection                                 */
        /**********************************************/
    }
    while (new_sd >= 0);
}
/*****************************************************************************/
void TcpServer::IncommingData(Conn &conn)
{
    TcpSocket socket(conn.peer);

    // printf("  Descriptor %d is readable\n", in_sock);
    /*************************************************/
    /* Receive all incoming data on this socket      */
    /* before we loop back and call select again.    */
    /*************************************************/

    /**********************************************/
    /* Receive data on this connection until the  */
    /* recv fails with EWOULDBLOCK.  If any other */
    /* failure occurs, we will close the          */
    /* connection.                                */
    /**********************************************/

    bool hasData = socket.Recv();
    if (hasData)
    {
        if (conn.peer.isWebSocket)
        {
            if (conn.IsClosed())
            {
                if (socket.ProceedWsHandshake())
                {
                    // Websocket handshake success, warn the application
                    conn.state = Conn::cStateConnected;
                    mEventHandler.NewConnection(conn);
                }
                else
                {
                    TLogError("Websocket handshake failure.");
                }
            }
            else
            {
                // Assume we are in the connected state, manage the frame to extract the data
                hasData = socket.DecodeWsData(conn);
            }
        }

        if (hasData)
        {
            // Send the received data
            socket.DeliverData(conn);
            mEventHandler.ReadData(conn);
        }
    }
    else
    {
        conn.state = Conn::cStateDeleteLater;
    }
}
/*****************************************************************************/
void TcpServer::UpdateClients()
{
    bool cleanUp = true;
    /*************************************************/
    /* Check to see if the connection has been       */
    /* closed by the client.                         */
    /* This clean up process includes removing the   */
    /* descriptor from the master set and            */
    /* determining the new maximum descriptor value  */
    /* based on the bits that are still turned on in */
    /* the master set.                               */
    /*************************************************/

    while (cleanUp)
    {
        bool haveDeleted = false;
        for (size_t i = 0; i < mClients.size(); i++)
        {
            Conn conn = mClients[i];
            if (conn.state == Conn::cStateDeleteLater)
            {
                FD_CLR((u_int)conn.peer.socket, &mMasterSet); // need a cast here because of the macro

                mClients.erase(mClients.begin() + i);

                // Signal the disconnection
                // In case of the websocket, only warn upper layers if the handshake process has been done
                if (conn.IsConnected())
                {
                    mEventHandler.ClientClosed(conn);
                }
                conn.payload.clear();
                conn.state = Conn::cStateClosed;

                haveDeleted = true;
                break; // we have modified the array so we can't continue, leave the loop!!
            }
        }
        cleanUp = haveDeleted; // continue cleanup
    }

    UpdateMaxSocket();
}
/*****************************************************************************/
void TcpServer::UpdateMaxSocket()
{
    std::vector<Conn>::iterator pos;

    // find the maximum socket file descriptor with the clients
    pos = std::max_element(mClients.begin(), mClients.end());

    // Find max socket of the servers
    mMaxSd = mTcpServer.GetSocket();
    if (mWsServer.GetSocket() > mMaxSd)
    {
        mMaxSd = mWsServer.GetSocket();
    }

	if (pos != mClients.end())
	{
		if (*pos > mMaxSd)
		{
            mMaxSd = pos->peer.socket;
		}
	}
}

} // namespace tcp

//=============================================================================
// End of file TcpServer.cpp
//=============================================================================
