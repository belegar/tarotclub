/*=============================================================================
 * TarotClub - TcpSocket.h
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

#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#ifdef USE_UNIX_OS

// FIXME: use __GNUC__ macro for GCC under Linux
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#endif

#ifdef USE_WINDOWS_OS
#include <ws2tcpip.h>

#ifdef __MINGW32__
#include <unistd.h>
#endif

#ifdef _MSC_VER
#include <io.h>
#endif

#endif // USE_WINDOWS_OS

#include <cstdint>
#include <string>
#include "ByteArray.h"

/*****************************************************************************/
struct Peer
{
    Peer()
        : socket(-1)
        , isWebSocket(false)
    {

    }

    Peer(std::int32_t s, bool ws)
        : socket(s)
        , isWebSocket(ws)
    {

    }

    bool IsValid() const
    {
        return socket != -1;
    }

    bool operator == (const Peer &rhs)
    {
        bool ret = false;
        if ((socket == rhs.socket) && (isWebSocket == rhs.isWebSocket))
        {
            ret = true;
        }
        return ret;
    }

    std::int32_t socket;
    bool isWebSocket;
};
/*****************************************************************************/
class TcpSocket
{
public:
    // Websocket opcodes, from http://tools.ietf.org/html/rfc6455
    static const std::uint8_t WEBSOCKET_OPCODE_CONTINUATION     = 0x00U;
    static const std::uint8_t WEBSOCKET_OPCODE_TEXT             = 0x01U;
    static const std::uint8_t WEBSOCKET_OPCODE_BINARY           = 0x02U;
    static const std::uint8_t WEBSOCKET_OPCODE_CONNECTION_CLOSE = 0x08U;
    static const std::uint8_t WEBSOCKET_OPCODE_PING             = 0x09U;
    static const std::uint8_t WEBSOCKET_OPCODE_PONG             = 0x0AU;

    TcpSocket();
    TcpSocket(const Peer &peer);
    virtual ~TcpSocket();

    // Getters
    int  GetSocket() const
    {
        return mPeer.socket;
    }
    int  GetIPAddr() const
    {
        return mAddr.sin_addr.s_addr;
    }
    int  GetRealPort() const
    {
        return mPort;
    }

    // Setters
    bool SetBlocking(bool block);

    // Helpers
    bool IsValid() const
    {
        return mPeer.IsValid();
    }

    bool Create();
    bool Bind(std::uint16_t port, bool localHostOnly);
    void Close();
    bool Listen(std::int32_t maxConnections) const;
    bool HostNameToIpAddress(const std::string &address, sockaddr_in &ipv4);
    // return true if socket has data waiting to be read
    bool DataWaiting(uint32_t timeout);
    int AnalyzeSocketError(const char* context);
    int Accept() const;
    std::int32_t Recv(ByteArray &output);
    bool Connect(const std::string &host, const int port);
    bool Send(const ByteArray &input) const;

    // Static
    static bool Initialize();
    static int AnalyzeSocketError(const Peer &peer, const char* context);
    static bool Send(const ByteArray &input, const Peer &peer);
    static void Close(Peer &peer);
    static ByteArray BuildWsFrame(std::uint8_t opcode, const ByteArray &data);

protected:
    std::string mHost;
    std::uint16_t mPort;
    Peer mPeer;
    sockaddr_in mAddr;
    static bool mOneTimeInit;

private:
    static bool SendToSocket(const ByteArray &input, std::int32_t socket);
};

#endif // TCPSOCKET_H

//=============================================================================
// End of file TcpSocket.h
//=============================================================================
