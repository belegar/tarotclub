#ifndef TCP_SERVER_BASE_H
#define TCP_SERVER_BASE_H

#include "TcpSocket.h"

class TcpServerBase : public TcpSocket
{
public:
    TcpServerBase();

    bool CreateServer(std::uint16_t port, bool localHostOnly, int32_t maxConnections);
};

#endif // TCP_SERVER_BASE_H
