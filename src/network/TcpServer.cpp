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
    char *ipstr = NULL;
    int port = 0;

    len = sizeof(addr);
    int ret = getpeername(s, (struct sockaddr*)&addr, &len);

    // deal with IPv4
    if ((addr.ss_family == AF_INET) && (ret == 0))
    {
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        port = ntohs(s->sin_port);
        ipstr = inet_ntoa(s->sin_addr); // point to an internal static buffer
    }
    // FIXME: inet_ntoa is deprecated. Use inet_ntop instead, with IPv6 support
    // See the Boost implementation for Windows

    if (ipstr != NULL)
    {
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
                        if (FD_ISSET(mClients[j].socket, &working_set))
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
        if (FD_ISSET(mClients[i].socket, &mMasterSet))
        {
            TcpSocket::Close(mClients[i]);
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
        new_sd = -1;

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
            Peer newPeer(new_sd, webSocket);
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
bool TcpServer::IncommingData(Conn &conn)
{
    TcpSocket socket(conn);
    bool ret = false;
    int rc;
    ByteArray data;

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
    rc = socket.Recv(data);

    if (rc > 0)
    {
        ret = true;

        if (conn.isWebSocket)
        {
            ManageWsData(conn, data);
        }
        else
        {
            // Send the received data
            mEventHandler.ReadData(conn, data);
        }
    }
    else if (rc == -2)
    {
        // Try again to read from socket
    }
    else
    {
        /*************************************************/
        /* Check to see if the connection has been       */
        /* closed by the client.                         */
        /* This clean up process includes removing the   */
        /* descriptor from the master set and            */
        /* determining the new maximum descriptor value  */
        /* based on the bits that are still turned on in */
        /* the master set.                               */
        /*************************************************/
        socket.Close();

        for (size_t i = 0; i < mClients.size(); i++)
        {
            if (mClients[i] == conn.socket)
            {
                mClients.erase(mClients.begin() + i);
            }
        }
        FD_CLR((u_int)conn.socket, &mMasterSet); // need a cast here because of the macro
        UpdateMaxSocket();

        // Signal the disconnection
        // In case of the websocket, only warn upper layers if the handshake process has been done
        if (conn.IsConnected())
        {
            mEventHandler.ClientClosed(conn);
        }
        conn.wsPayload.Clear();
        conn.state = Conn::cStateClosed;
    }

    return ret;
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
            mMaxSd = pos->socket;
		}
	}
}
/*****************************************************************************/
void TcpServer::ManageWsData(Conn &conn, const ByteArray &data)
{
    if (conn.IsClosed())
    {
        // Process the handshake, upgrade into our own protocol
        WebSocketRequest ws;

        ws.Parse(data.ToSring());
        if (ws.IsValid())
        {
            // Trick here: send handshake using raw tcp, not with websocket framing
            Peer peer = conn;
            peer.isWebSocket = false;
            TcpSocket::Send(ws.Upgrade("tarotclub"), peer);
            conn.state = Conn::cStateConnected;
            conn.wsPayload.Clear();
            mEventHandler.NewConnection(conn);
        }
    }
    else
    {
        // Assume we are in the connected state, manage the frame to extract the data
        DeliverWsData(conn, data);
    }
}
/*****************************************************************************/
/*
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-------+-+-------------+-------------------------------+
|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
|N|V|V|V|       |S|             |   (if payload len==126/127)   |
| |1|2|3|       |K|             |                               |
+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
|     Extended payload length continued, if payload len == 127  |
+ - - - - - - - - - - - - - - - +-------------------------------+
|                               |Masking-key, if MASK set to 1  |
+-------------------------------+-------------------------------+
| Masking-key (continued)       |          Payload Data         |
+-------------------------------- - - - - - - - - - - - - - - - +
:                     Payload Data continued ...                :
+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
|                     Payload Data continued ...                |
+---------------------------------------------------------------+
*/
void TcpServer::DeliverWsData(Conn &conn, const ByteArray &data)
{
    ByteArray copy = data;
    // Having buf unsigned char * is important, as it is used below in arithmetic
    unsigned char *buf = copy.Data();
    size_t i, len, mask_len = 0, header_len = 0, data_len = 0;

    std::uint32_t buf_len = data.Size();

    /* Extracted from the RFC 6455 Chapter 5-2
     *
    The length of the "Payload data", in bytes: if 0-125, that is the
    payload length.  If 126, the following 2 bytes interpreted as a
    16-bit unsigned integer are the payload length.  If 127, the
    following 8 bytes interpreted as a 64-bit unsigned integer (the
    most significant bit MUST be 0) are the payload length.  Multibyte
    length quantities are expressed in network byte order.  Note that
    in all cases, the minimal number of bytes MUST be used to encode
    the length, for example, the length of a 124-byte-long string
    can't be encoded as the sequence 126, 0, 124.  The payload length
    is the length of the "Extension data" + the length of the
    "Application data".  The length of the "Extension data" may be
    zero, in which case the payload length is the length of the
    "Application data". */
    if (buf_len >= 2)
    {
        len = buf[1] & 127;
        mask_len = buf[1] & 128 ? 4 : 0;
        if (len < 126 && buf_len >= mask_len)
        {
            data_len = len;
            header_len = 2 + mask_len;
        }
        else if (len == 126 && buf_len >= 4 + mask_len)
        {
            header_len = 4 + mask_len;
            data_len = ((((size_t) buf[2]) << 8) + buf[3]);
        }
        else if (buf_len >= 10 + mask_len)
        {
            header_len = 10 + mask_len;
            data_len = (size_t) (((uint64_t) htonl(* (uint32_t *) &buf[2])) << 32) + htonl(* (uint32_t *) &buf[6]);
        }
    }

    // frame_len = header_len + data_len;
    // Apply mask if necessary
    if (mask_len > 0)
    {
        for (i = 0; i < data_len; i++)
        {
            buf[i + header_len] ^= (buf + header_len - mask_len)[i % 4];
        }
    }

    std::uint8_t opcode = buf[0] & 0xFU;
    bool FIN = (buf[0] & 0x80U) == 0x80U;
    TLogNetwork("received opcode: " + WsOpcodeToString(opcode));

    /*
    Manage fragmentation here: extract from the RFC:

    EXAMPLE: For a text message sent as three fragments, the first
      fragment would have an opcode of 0x1 and a FIN bit clear, the
      second fragment would have an opcode of 0x0 and a FIN bit clear,
      and the third fragment would have an opcode of 0x0 and a FIN bit
      that is set.

      */
    if(opcode == WEBSOCKET_OPCODE_PING)
    {
        // FIXME: send pong!
    }
    else if (opcode == WEBSOCKET_OPCODE_CONNECTION_CLOSE)
    {
        conn.wsPayload.Clear();
        conn.state = Conn::cStateClosed;
    }
    else
    {
        if ((opcode == WEBSOCKET_OPCODE_TEXT) ||
            (opcode == WEBSOCKET_OPCODE_BINARY))
        {
            conn.wsPayload.Clear();
            conn.wsPayload += copy.SubArray(header_len, data_len);
        }
        else if(opcode == WEBSOCKET_OPCODE_CONTINUATION)
        {
            conn.wsPayload += copy.SubArray(header_len, data_len);
        }

        if (FIN)
        {
            // Send the received data
            mEventHandler.ReadData(conn, conn.wsPayload);
            TLogNetwork("WebSocket data: " + conn.wsPayload.ToSring());
        }
    }
}
/*****************************************************************************/
std::string TcpServer::WsOpcodeToString(std::uint8_t opcode)
{
    std::string ocString;
    if (opcode == WEBSOCKET_OPCODE_CONTINUATION)
    {
        ocString = "WebSocket opcode: continuation";
    }
    else if (opcode == WEBSOCKET_OPCODE_TEXT)
    {
        ocString = "WebSocket opcode: text data";
    }
    else if (opcode == WEBSOCKET_OPCODE_BINARY)
    {
        ocString = "WebSocket opcode: binary data";
    }
    else if (opcode == WEBSOCKET_OPCODE_CONNECTION_CLOSE)
    {
        ocString = "WebSocket opcode: connection close";
    }
    else if (opcode == WEBSOCKET_OPCODE_PING)
    {
        ocString = "WebSocket opcode: ping";
    }
    else if (opcode == WEBSOCKET_OPCODE_PONG)
    {
        ocString = "WebSocket opcode: pong";
    }
    else
    {
        ocString = "Unkown WebSocket opcode";
    }
    return ocString;
}


//=============================================================================
// End of file TcpServer.cpp
//=============================================================================
