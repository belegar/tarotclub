/*=============================================================================
 * TarotClub - TcpClient.cpp
 *=============================================================================
 * TCP socket wrapper class, client side
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

#include "TcpClient.h"

namespace tcp
{

/*****************************************************************************/
TcpClient::TcpClient()
{

}
/*****************************************************************************/
bool TcpClient::Initialize()
{
    /*************************************************************/
    /* Create an AF_INET stream socket                           */
    /*************************************************************/
    if (!Create())
    {
        return false;
    }

    if (!SetBlocking(false))
    {
        return false;
    }

    return true;
}
/*****************************************************************************/
bool TcpClient::Connect(const std::string &host, const int port)
{
    bool ret = false;

    mHost = host;
    mPort = port;

    if (HostNameToIpAddress(host, mAddr))
    {
   //     mAddr.sin_family = AF_INET;
   //     mAddr.sin_port = htons(port);
   //     mAddr.sin_addr.s_addr = inet_addr(mHost.c_str()); // Convert a string IPv4 into a structure

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
        mAddr.sin_port = htons(mPort);
        int retcode = ::connect(mPeer.socket, reinterpret_cast<sockaddr *>(&mAddr), sizeof(mAddr));
        if (retcode == 0)
        {
            ret = true;
        }
        else if (retcode < 0)
        {
            /* This is what we expect for non-blocking connect. */
            if (AnalyzeSocketError(mPeer, "connect()"))
            {
                fd_set wrfds;
                struct timeval tout;

                tout.tv_sec = 5;
                tout.tv_usec = 0;

                FD_ZERO(&wrfds);

                FD_SET(mPeer.socket, &wrfds);

                // Wait for connection ...
                int result = select(mPeer.socket + 1, NULL, &wrfds, NULL, &tout); // FD_SETSIZE ?

                // Do we have one connection?
                if (result == 1)
                {
                    ret = IsConnected();
                }
            }
        }
    }

    return ret;
}
/*****************************************************************************/
bool TcpClient::DataWaiting(std::uint32_t timeout)
{
    fd_set fds;
    FD_ZERO( &fds );
    FD_SET( mPeer.socket, &fds );

    struct timeval tv;
    tv.tv_sec = (long)timeout;
    tv.tv_usec = 0;

    bool ok = false;
    int r = select( mPeer.socket + 1, &fds, NULL, NULL, &tv);
    if (r < 0)
    {
        ok = AnalyzeSocketError("select()");
    }
    else
    {
        if( FD_ISSET( mPeer.socket, &fds ) )
        {
            ok = true;
        }
    }
    return ok;
}


} // namespace tcp

//=============================================================================
// End of file TcpClient.cpp
//=============================================================================
