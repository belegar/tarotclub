/*=============================================================================
 * TarotClub - TcpSocket.cpp
 *=============================================================================
 * Multi OS simple TCP socket wrapper class
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

#include "TcpSocket.h"


#include <errno.h>  // errno, just like it says.
#include <fcntl.h>  // symbolic names for socket flags.

#include <cstring>
#include <iostream>

// Larger values will read larger chunks of data.
static const std::int32_t MAXRECV = 4096;


bool TcpSocket::mOneTimeInit = false;

/*****************************************************************************/
TcpSocket::TcpSocket()
    : mHost("127.0.0.1")
    , mPort(0U)
    , mSock(-1)
{
    std::memset(&mAddr, 0, sizeof(mAddr));
}
/*****************************************************************************/
TcpSocket::TcpSocket(int sock)
    : mHost("127.0.0.1")
    , mPort(0U)
    , mSock(sock)
{
    memset(&mAddr, 0, sizeof(mAddr));
}
/*****************************************************************************/
TcpSocket::~TcpSocket()
{

}
/*****************************************************************************/
/**
 * @brief TcpSocket::Create
 * Server initialization
 * @return
 */
bool TcpSocket::Create()
{
    bool ret = false;
    mSock = socket(AF_INET, SOCK_STREAM, 0);

    if (IsValid())
    {
        std::int32_t on = 1;

        // Allows the socket to be bound to an address that is already in use.
        // For more information, see bind. Not applicable on ATM sockets.
        if (setsockopt(mSock,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       reinterpret_cast<const char *>(&on),
                       sizeof(on)) == 0)
        {
            ret = true;
        }
    }
    return ret;
}
/*****************************************************************************/
bool TcpSocket::SetBlocking(bool block)
{
    unsigned long on = 1;
    bool ret = false;

    if (block == true)
    {
        on = 0;
    }

#ifdef USE_UNIX_OS
    std::int32_t rc = ioctl(mSock, FIONBIO, (char *)&on);
#else
    std::int32_t rc = ioctlsocket(mSock, FIONBIO, &on);
#endif
    if (rc == 0)
    {
        ret = true;
    }
    return ret;
}
/*****************************************************************************/
bool TcpSocket::Bind(std::uint16_t port)
{
    bool ret = false;

    memset(&mAddr, 0, sizeof(mAddr));

    if (IsValid())
    {
        mAddr.sin_family      = AF_INET;
        mAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        mAddr.sin_port        = htons(port);

        if (::bind(mSock,
                   reinterpret_cast<struct sockaddr *>(&mAddr),
                   sizeof(mAddr)) == 0)
        {
            ret = true;
        }
    }
    return ret;
}
/*****************************************************************************/
void TcpSocket::Close()
{
    if (IsValid())
    {
#ifdef USE_UNIX_OS
        ::shutdown(mSock, SHUT_RDWR);
        ::close(mSock);
#else
        ::shutdown(mSock, SD_BOTH);
        ::closesocket(mSock);
#endif
    }
}
/*****************************************************************************/
bool TcpSocket::Listen(std::int32_t maxConnections) const
{
    bool ret = false;

    if (IsValid())
    {
        if (::listen(mSock, maxConnections) == 0)
        {
            ret = true;
        }
    }
    return ret;
}
/*****************************************************************************/
int TcpSocket::Accept() const
{
    int new_sd = ::accept(mSock, NULL, NULL);
    return new_sd;
}
/*****************************************************************************/
bool TcpSocket::Connect(const std::string &host, const int port)
{
    bool ret = false;

    mHost = host;
    mPort = port;
    mAddr.sin_family = AF_INET;
    mAddr.sin_port = htons(port);
    mAddr.sin_addr.s_addr = inet_addr(mHost.c_str()); // Convert a string IPv4 into a structure

    if (mAddr.sin_addr.s_addr == INADDR_NONE)
    {
        //printf("inet_addr failed and returned INADDR_NONE\n");
        //     WSACleanup();
        return false;
    }
    if (mAddr.sin_addr.s_addr == INADDR_ANY)
    {
        //    printf("inet_addr failed and returned INADDR_ANY\n");
        return false;
    }

    int retcode = ::connect(mSock, reinterpret_cast<sockaddr *>(&mAddr), sizeof(mAddr));
    if (retcode == 0)
    {
        ret = true;
    }

    return ret;
}
/*****************************************************************************/
bool TcpSocket::Send(const std::string &input) const
{
    size_t ret;

    ret = ::send(mSock, input.c_str(), input.size(), 0);

    if (ret > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
/*****************************************************************************/
/**
 * @brief TcpSocket::Initialize
 * One time initialization for the sub-system
 * @return
 */
bool TcpSocket::Initialize()
{
    if (!mOneTimeInit)
    {
        mOneTimeInit = true;
#ifdef USE_WINDOWS_OS
        WSADATA wsaData;

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
        {
            return false;
        }
#endif
    }

    return true;
}
/*****************************************************************************/
std::int32_t TcpSocket::Recv(std::string &output) const
{
    char buf [MAXRECV + 1];
    memset(buf, 0, sizeof(buf));


    output = std::string();
    int status = 0; // changed from int to ssize_t

    // Most likely, we will read a packet, or if the message
    // is very short, we will receive the entire message in
    // a short packet. But it might be a long one.
    status = ::recv(mSock, buf, MAXRECV, 0);

    if (status > 0)
    {
        output.append(buf, static_cast<size_t>(status));
        memset(buf, 0, sizeof(buf));
    }

    // return the status code, or the number of bytes read.
    return status;
}

//=============================================================================
// End of file TcpSocket.cpp
//=============================================================================
