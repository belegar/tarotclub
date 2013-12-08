#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>
#include "TcpSocket.h"

class TcpClient : public TcpSocket
{
public:

    TcpClient ();
    ~TcpClient() { }

    bool Start();

};

#endif // TCPCLIENT_H
