#ifndef TCP_SERVER_BASE_H
#define TCP_SERVER_BASE_H

#include "TcpSocket.h"

namespace tcp
{

class TcpServerBase : public TcpSocket
{
public:
    TcpServerBase();

    bool CreateServer(std::uint16_t port, bool localHostOnly, int32_t maxConnections);
};

} // namespace tcp

#endif // TCP_SERVER_BASE_H
