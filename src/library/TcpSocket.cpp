
#include "TcpSocket.h"

/*****************************************************************************/
SocketError::SocketError(const std::string & Message,
            const char * TranslationUnitName,
            int LineNumber,
            bool Fatal)
{
    fileName = TranslationUnitName != 0 ? TranslationUnitName : std::string();
    m_Message = Message;
    m_Fatal = Fatal;
    m_LineNumber = LineNumber;
}
/*****************************************************************************/
void SocketError::tombstone(std::ostream & Out)
{
    Out << std::endl <<
     (m_Fatal ? "SocketError reports an unrecoverable error has occurred." :
                "System message from SocketError ") << std::endl;
    Out << m_Message << std::endl;

    if (fileName != std::string())
    Out << "Problem detected near line " << m_LineNumber << " of file "
        << fileName << std::endl;
}


/*****************************************************************************/
TcpSocket::TcpSocket()
    : mHost("127.0.0.1")
    , mPort(0U)
    , mSock(-1)
{
    memset (&mAddr, 0, sizeof(mAddr));
}
/*****************************************************************************/
TcpSocket::~TcpSocket()
{
    if (IsValid()) ::close (mSock);
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
        if (setsockopt (mSock,
                         SOL_SOCKET,
                         SO_REUSEADDR,
                         reinterpret_cast<const char*>(&on),
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
        mAddr.sin_addr.s_addr = htonl(INADDR_ANY);
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
        ::shutdown(mSock, SD_BOTH);

#ifdef USE_UNIX_OS
        ::close(mSock);
#else
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
bool TcpSocket::Accept (int &new_sd) const
{
    bool ret = false;

    new_sd = ::accept(mSock, NULL, NULL);
    if (new_sd > 0)
    {
       ret = true;
    }
    return ret;
}
/*****************************************************************************/
bool TcpSocket::Connect (const std::string &host, const int port)
{
    bool ret = false;

    mHost = host;
    mPort = port;
    mAddr.sin_family = AF_INET;
    mAddr.sin_port = htons (port);

    // Convert a string IPv4 into a structure
    unsigned long ulAddr = INADDR_NONE;
    ulAddr  = inet_addr(mHost.c_str());

    if ( ulAddr == INADDR_NONE )
    {
        //printf("inet_addr failed and returned INADDR_NONE\n");
        //     WSACleanup();
        return false;
    }

    if (ulAddr == INADDR_ANY)
    {
    //    printf("inet_addr failed and returned INADDR_ANY\n");
        return false;
    }
    mAddr.sin_addr.S_un.S_addr = ulAddr;

    if (::connect(mSock, reinterpret_cast<sockaddr *>(&mAddr), sizeof(mAddr)) == 0)
    {
        ret = true;
    }
   // if (status) m_lastError = errno;

    return ret;
}
/*****************************************************************************/
bool TcpSocket::Send (const std::string & input) const
{
    return ::send(mSock, input.c_str(), input.size(), 0) != -1;
}

/**
 * @brief TcpSocket::Initialize
 * One time initialization for the sub-system
 * @return
 */
bool TcpSocket::Initialize()
{
#ifdef USE_WINDOWS_OS
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
    {
        return false;
    }
#endif

    return true;
}
/*****************************************************************************/
std::int32_t TcpSocket::Recv (std::string & output) const
{
    char buf [MAXRECV + 1];
    memset(buf, 0, sizeof(buf));


    output = std::string();
    ssize_t status = 0; // changed from int to ssize_t

    // Most likely, we will read a packet, or if the message
    // is very short, we will receive the entire message in
    // a short packet. But it might be a long one.
    status = ::recv(mSock, buf, MAXRECV, 0);//MSG_WAITALL );

    if(status > 0)
    {
        output.append(buf, static_cast<size_t>(status));
        memset(buf, 0, sizeof(buf));
    }

    // return the status code, or the number of bytes read.
    return status;
}


