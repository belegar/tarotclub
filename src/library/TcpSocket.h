#ifndef TCPSOCKET_H
#define TCPSOCKET_H

// C level network includes

#ifdef USE_UNIX_OS

// FIXME: use __GNUC__ macro for GCC under Linux
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
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
class TcpSocket
{
public:
    TcpSocket();
    virtual ~TcpSocket();

    // Getters
    int  GetSocket() const      { return mSock; }
    int  GetIPAddr() const      { return mAddr.sin_addr.s_addr; }
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

    // Static
    static bool Initialize();

private:
    std::string mHost;
    std::uint16_t mPort;
    int mSock;
    sockaddr_in mAddr;
    static bool mInitialized;
};

#endif // TCPSOCKET_H
