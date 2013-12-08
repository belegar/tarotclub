#ifndef TCPSOCKET_H
#define TCPSOCKET_H

// C level network includes

#ifdef USE_UNIX_OS
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#else
#include <ws2tcpip.h>
#endif

#include <unistd.h> // posix
#include <string.h> // memset.
#include <errno.h>  // errno, just like it says.
#include <fcntl.h>  // symbolic names for socket flags.

// C++ creatures
#include <string>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <exception>
#include <cstdint>

// Larger values will read larger chunks of data.
#define MAXRECV (1024)

/*****************************************************************************/
class SocketError
{
public:
    SocketError(const std::string & Message, const char * TranslationUnitName = 0, int LineNumber = 0, bool Fatal = true);

    virtual ~SocketError(void) { }
    virtual void tombstone(std::ostream & Out = std::cerr);

private:
    std::string m_Message;
    bool m_Fatal;
    std::string fileName;
    int m_LineNumber;
};

/*****************************************************************************/
class TcpSocket
{
public:
    TcpSocket();
    virtual ~TcpSocket();

    // Getters
    int  GetSocket() const      { return mSock; }
    int  GetIPAddr() const      { return mAddr.sin_addr.s_addr; }
    int  GetLastError() const   { return m_lastError; }
    int  GetRealPort() const    { return mPort; }

    // Setters
    bool SetBlocking(bool block);
    void SetSocket(int sock) { mSock = sock; }

    // Helpers
    bool IsValid() const { return mSock != -1; }

    bool Create();
    bool Bind(std::uint16_t port);
    void Close();
    bool Listen(std::int32_t maxConnections) const;
    bool Accept(int &new_sd) const;
    std::int32_t Recv(std::string & output) const;
    bool Connect(const std::string &host, const int port);
    bool Send(const std::string & input) const;

    static bool Initialize();

private:
    std::string mHost;
    std::uint16_t mPort;
    int mSock;
    sockaddr_in mAddr;
    int m_lastError;
};

#endif // TCPSOCKET_H
