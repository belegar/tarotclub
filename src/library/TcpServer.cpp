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
#include "TcpServer.h"

/*****************************************************************************/
TcpServer::TcpServer(IEvent &handler)
    : mInitialized(false)
    , mEventHandler(handler)
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
    Close();
    mThread.join();
    mInitialized = false;
}
/*****************************************************************************/
void TcpServer::Run()
{
    int rc;
    bool end_server = false;
    struct timeval timeout;
    fd_set working_set;

    for (;;)
    {
        /*************************************************************/
        /* Initialize the master fd_set                              */
        /*************************************************************/
        FD_ZERO(&mMasterSet);
        mMaxSd = GetSocket();
        FD_SET(GetSocket(), &mMasterSet);

        /*************************************************************/
        /* Initialize the timeval struct to 3 minutes.  If no        */
        /* activity after 3 minutes this program will end.           */
        /*************************************************************/
        timeout.tv_sec  = 3 * 60;
        timeout.tv_usec = 0;

        /*************************************************************/
        /* Loop waiting for incoming connects or for incoming data   */
        /* on any of the connected sockets.                          */
        /*************************************************************/
        do
        {
            /**********************************************************/
            /* Copy the master fd_set over to the working fd_set.     */
            /**********************************************************/
            memcpy(&working_set, &mMasterSet, sizeof(mMasterSet));

            /**********************************************************/
            /* Call select() and wait 5 minutes for it to complete.   */
            /**********************************************************/
            printf("Waiting on select()...\n");
            rc = select(mMaxSd + 1, &working_set, NULL, NULL, &timeout);

            if (rc < 0)
            {
                /**********************************************************/
                /* The select call failed.                                */
                /**********************************************************/
                end_server = true;
            }
            else if (rc == 0)
            {
                /**********************************************************/
                /* The time out expired.                                  */
                /**********************************************************/
                //TODO: call a listener / signal this event
            }
            else
            {
                /**********************************************************/
                /* One or more descriptors are readable.  Need to         */
                /* determine which ones they are.                         */
                /**********************************************************/
                for (int i = 0; i < rc; i++)
                {
                    if (FD_ISSET(GetSocket(), &working_set))
                    {
                        /****************************************************/
                        /* This is the listening socket                     */
                        /****************************************************/
                        IncommingConnection();
                    }
                    else
                    {
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
                    }
                }
            }
        }
        while (end_server == false);

        /*************************************************************/
        /* Cleanup all of the sockets that are open                  */
        /*************************************************************/
        for (size_t i = 0; i < mClients.size(); i++)
        {
            if (FD_ISSET(mClients[i], &mMasterSet))
            {
                TcpSocket socket;
                socket.SetSocket(i);
                socket.Close();
            }
        }

        return;
    }
}
/*****************************************************************************/
void TcpServer::EntryPoint(void *pthis)
{
    TcpServer * pt = (TcpServer*)pthis;
    pt->Run();
}
/*****************************************************************************/
void TcpServer::IncommingConnection()
{
    int new_sd = -1;

    printf("  Listening socket is readable\n");
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
        if (!Accept(new_sd))
        {
            break;
        }

        mClients.push_back(new_sd);

        /**********************************************/
        /* Add the new incoming connection to the     */
        /* master read set                            */
        /**********************************************/
        printf("  New incoming connection - %d\n", new_sd);
        FD_SET(new_sd, &mMasterSet);

        // Update the maximum socket file identifier
        UpdateMaxSocket();

        // Signal a new client
        mEventHandler.NewConnection(new_sd);

        /**********************************************/
        /* Loop back up and accept another incoming   */
        /* connection                                 */
        /**********************************************/
    } while (new_sd != -1);
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

    printf("  Descriptor %d is readable\n", in_sock);
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
        mEventHandler.ReadData(buffer);
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

        for (size_t i=0; i<mClients.size(); i++)
        {
            if (mClients[i] == in_sock)
            {
                mClients.erase(mClients.begin()+i);
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
    pos = std::max_element (mClients.begin(), mClients.end());

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
