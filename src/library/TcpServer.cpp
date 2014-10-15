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
#include <iostream>
#include <sstream>

/*****************************************************************************/
TcpServer::TcpServer(IEvent &handler)
    : mMaxSd(0)
    , mInitialized(false)
    , mEventHandler(handler)
    , mReceiveFd(-1)
    , mSendFd(-1)
{
}
/*****************************************************************************/
bool TcpServer::Start(std::uint16_t port, std::int32_t maxConnections)
{
    /*************************************************************/
    /* Create an AF_INET stream socket to receive incoming       */
    /* connections on                                            */
    /*************************************************************/
    if (!Create())
    {
        return false;
    }

    /*************************************************************/
    /* Set socket to be non-blocking.  All of the sockets for    */
    /* the incoming connections will also be non-blocking since  */
    /* they will inherit that state from the listening socket.   */
    /*************************************************************/
    if (!SetBlocking(false))
    {
        Close();
        return false;
    }

    /*************************************************************/
    /* Bind the socket                                           */
    /*************************************************************/
    if (!Bind(port))
    {
        Close();
        return false;
    }

    /*************************************************************/
    /* Set the listen back log                                   */
    /*************************************************************/
    if (!Listen(maxConnections))
    {
        Close();
        return false;
    }

    // Create the thread the first time only
    if (!mInitialized)
    {
        mThread = std::thread(TcpServer::EntryPoint, this);
        mInitialized = true;
    }

    return true;
}
/*****************************************************************************/
void TcpServer::Stop()
{
    if (mSendFd > 0)
    {
        write(mSendFd, "1", 1);
    }
    Close();
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
    char ipv6str[INET6_ADDRSTRLEN];
    std::memset(ipv6str, 0, INET6_ADDRSTRLEN);
    char *ipstr = ipv6str;
    int port;

    len = sizeof(addr);
    getpeername(s, (struct sockaddr*)&addr, &len);

    // deal with IPv4
    if (addr.ss_family == AF_INET)
    {
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        port = ntohs(s->sin_port);
        ipstr = inet_ntoa(s->sin_addr); // point to an internal static buffer
    }
    // FIXME: inet_ntoa is deprecated. Use inet_ntop instead, with IPv6 support
    // See the Boost implementation for Windows

    ss << ipstr << ":" << port;

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
    mMaxSd = GetSocket();
    FD_SET(mMaxSd, &mMasterSet);

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
            end_server = true;
            mEventHandler.ServerTerminated(IEvent::WAIT_SOCK_FAILED);
        }
        else if (rc == 0)
        {
            /**********************************************************/
            /* The time out expired.                                  */
            /**********************************************************/
            end_server = true;
            mEventHandler.ServerTerminated(IEvent::TIMEOUT);
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
                else if (FD_ISSET(GetSocket(), &working_set))
                {
                    /****************************************************/
                    /* This is the listening socket                     */
                    /****************************************************/
                    mMutex.lock();
                    IncommingConnection();
                    mMutex.unlock();
                }
                else
                {
                    mMutex.lock();

                    // Scan for already connected clients
                    for (size_t j = 0; j < mClients.size(); j++)
                    {
                        if (FD_ISSET(mClients[j], &working_set))
                        {
                            /****************************************************/
                            /* This is not the listening socket, therefore an   */
                            /* existing connection must be readable             */
                            /****************************************************/
                            IncommingData(mClients[j]);
                        }
                    }
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
        if (FD_ISSET(mClients[i], &mMasterSet))
        {
            TcpSocket socket;
            socket.SetSocket(i);
            socket.Close();
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
void TcpServer::IncommingConnection()
{
    int new_sd;

    //    printf("  Listening socket is readable\n");
    /*************************************************/
    /* Accept all incoming connections that are      */
    /* queued up on the listening socket before we   */
    /* loop back and call select again.              */
    /*************************************************/
    do
    {
        new_sd = -1;

        /**********************************************/
        /* Accept each incoming connection.  If       */
        /* accept fails with EWOULDBLOCK, then we     */
        /* have accepted all of them.  Any other      */
        /* failure on accept will cause us to end the */
        /* server.                                    */
        /**********************************************/
        new_sd = Accept();

        if (new_sd > 0)
        {
            // Save socket descriptor
            mClients.push_back(new_sd);

            /**********************************************/
            /* Add the new incoming connection to the     */
            /* master read set                            */
            /**********************************************/
            //     printf("  New incoming connection - %d\n", new_sd);
            FD_SET(new_sd, &mMasterSet);

            // Update the maximum socket file identifier
            UpdateMaxSocket();

            // Signal a new client
            mEventHandler.NewConnection(new_sd);
        }

        /**********************************************/
        /* Loop back up and accept another incoming   */
        /* connection                                 */
        /**********************************************/
    }
    while (new_sd > 0);
}
/*****************************************************************************/
bool TcpServer::IncommingData(int in_sock)
{
    bool close_conn = false;
    TcpSocket socket;
    bool ret = false;
    std::string buffer;
    int rc;

    socket.SetSocket(in_sock);

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
    rc = socket.Recv(buffer);

    if (rc > 0)
    {
        ret = true;

        // Send the received data
        mEventHandler.ReadData(in_sock, buffer);
    }
    else if (rc < 0)
    {
        // Receive failure
        close_conn = true;
    }
    else if (rc == 0)
    {
        /**********************************************/
        /* Check to see if the connection has been    */
        /* closed by the client                       */
        /**********************************************/
        close_conn = true;
    }

    /*************************************************/
    /* If the close_conn flag was turned on, we need */
    /* to clean up this active connection.  This     */
    /* clean up process includes removing the        */
    /* descriptor from the master set and            */
    /* determining the new maximum descriptor value  */
    /* based on the bits that are still turned on in */
    /* the master set.                               */
    /*************************************************/
    if (close_conn)
    {
        socket.Close();

        for (size_t i = 0; i < mClients.size(); i++)
        {
            if (mClients[i] == in_sock)
            {
                mClients.erase(mClients.begin() + i);
            }
        }
        FD_CLR((u_int)in_sock, &mMasterSet); // need a cast here because of the macro
        UpdateMaxSocket();

        // Signal the disconnection
        mEventHandler.ClientClosed(in_sock);
    }

    return ret;
}
/*****************************************************************************/
void TcpServer::UpdateMaxSocket()
{
    std::vector<int>::iterator pos;

    // find the maximum socket file descriptor
    pos = std::max_element(mClients.begin(), mClients.end());

    if (*pos > GetSocket())
    {
        mMaxSd = *pos;
    }
    else
    {
        mMaxSd = GetSocket();
    }
}

//=============================================================================
// End of file TcpServer.cpp
//=============================================================================
