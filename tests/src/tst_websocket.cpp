#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>

#include "WebSocket.h"
#include "tst_websocket.h"

TstWebSocket::TstWebSocket()
{
}


void TstWebSocket::Upgrade()
{

    // Test from RFC 6455...
    //$key=         "dGhlIHNhbXBsZSBub25jZQ=="
    //$cat_string=  "dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    //$resultBase64="s3pPLMBiTxaQ9kYGzzhZRbK+xOo=";

    std::string data = "GET / HTTP / 1.1\r\n";
    data += "Host: server.example.com\r\n";
    data += "Upgrade: websocket\r\n";
    data += "Connection: Upgrade\r\n";
    data += "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n";
    data += "Origin: http://example.com\r\n";
    data += "Sec-WebSocket-Protocol: superchat\r\n";

    WebSocketRequest req;
    req.Parse(data);

    if (req.IsValid())
    {
        std::string accept = req.Accept();
        if (accept == "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=")
        {
            std::cout << "Upgrade header: " << std::endl << req.Upgrade("tarotclub") << std::endl;
        }
        else
        {
            QFAIL("Invalid accept encoded string");
            std::cout << "Invalid accept encoded string: " << accept << std::endl;
        }
    }
    else
    {
        QFAIL("Invalid HTTP request");
        std::cout << "Invalid request" << std::endl;
    }
}
