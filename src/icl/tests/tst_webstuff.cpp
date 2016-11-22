#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>
#include <string>

#include "TcpSocket.h"
#include "WebSocket.h"
#include "tst_webstuff.h"
#include "CouchDb.h"

TstWebStuff::TstWebStuff()
{
}

void TstWebStuff::Http()
{
    tcp::TcpSocket::Initialize();
    std::string value;
    std::string expected;

    bool boolValue;
    bool boolExpected;
    CouchDb db;

    if (db.Connect())
    {
        db.Get("/");

        if (db.IsValid())
        {
            expected = "1.6.1";
            // key is at the root of the document
            if (!db.GetValue("version", value))
            {
                QFAIL("JSON Get value error");
            }
            QCOMPARE(value, expected);
        }
        else
        {
            QFAIL("Invalid reply");
        }

        // Delete any previously created test database
        db.Delete("/test");
        if (db.IsValid())
        {
            // We don't parse the reply because it depends if
            // the database was existing or not. The following tests will
            // fail later if anything goes wrong here, so don't bother.
        }
        else
        {
           QFAIL("Invalid reply");
        }

        // Get a document that does not exists
        db.Get("/test/foobar");
        if (db.IsValid())
        {
            expected = "not_found";
            if (!db.GetValue("error", value))
            {
                QFAIL("JSON Get value error");
            }
            QCOMPARE(value, expected);
        }
        else
        {
           QFAIL("Invalid reply");
        }

        // Create a database
        db.Put("/test");
        if (db.IsValid())
        {
            boolExpected = true;
            if (!db.GetValue("ok", boolValue))
            {
                QFAIL("JSON Get value error");
            }
            QCOMPARE(boolValue, boolExpected);
        }
        else
        {
           QFAIL("Invalid reply");
        }

        // Create a document
        JsonObject doc;
        doc.AddValue("coucou", 7);
        std::string docId = "mydoc";
        db.Put("/test/" + docId, doc.ToString(0U));
        if (db.IsValid())
        {
            boolExpected = true;
            if (!db.GetValue("ok", boolValue))
            {
                QFAIL("JSON Get value error");
            }
            QCOMPARE(boolValue, boolExpected);
            if (!db.GetValue("id", value))
            {
                QFAIL("JSON Get value error");
            }
            QCOMPARE(value, docId);
        }
        else
        {
           QFAIL("Invalid reply");
        }

        // Create a document without providing our own ID
        doc.Clear();
        doc.AddValue("hiphop", false);
        db.Post("/test", doc.ToString(0U));
        if (db.IsValid())
        {
            boolExpected = true;
            if (!db.GetValue("ok", boolValue))
            {
                std::cout << db.GetRawData() << std::endl;
                std::cout << doc.ToString(0U) << std::endl;
                QFAIL("JSON Get value error");
            }
        }
        else
        {
           QFAIL("Invalid reply");
        }
    }

}

void TstWebStuff::WebSocketUpgrade()
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

