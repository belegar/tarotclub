#include <iostream>
#include <thread>
#include <chrono>

#include "TcpClient.h"
#include "TcpServer.h"
#include "WebSocket.h"
#include "tst_tcp_socket.h"

/**
 * @brief Simple echo server
 */
class EchoServer : public tcp::TcpServer::IEvent
{

public:

    virtual void NewConnection(const tcp::Conn &conn)
	{
        (void) conn;
	}

    virtual void ReadData(const tcp::Conn &conn)
	{
		// echo the data received
        tcp::TcpSocket peer(conn.peer);
        peer.Send(conn.payload);
	}

    virtual void ClientClosed(const tcp::Conn &conn)
	{
        (void) conn;
	}

    virtual void ServerTerminated(tcp::TcpServer::IEvent::CloseType type)
    {
        (void) type;
    }

};

static bool echoTestSuccess = false;

void ClientThread()
{
    tcp::TcpClient client;
    std::string buff;
    std::string data = "Hello, world!";

    client.Initialize();
    client.Connect("127.0.0.1", 61617);

    std::this_thread::sleep_for(std::chrono::seconds(1U));
    if (client.Send(data))
    {
        if (client.DataWaiting(2U))
        {
            if (client.Recv(buff))
            {
                std::cout << "TEST Received: " << buff << std::endl;
                if (data == buff)
                {
                    echoTestSuccess = true;
                }
            }
            else
            {
                std::cout << "TEST Read error" << std::endl;
            }
        }
        else
        {
            std::cout << "TEST Client Timeout" << std::endl;
        }
    }
    else
    {
        std::cout << "Write error" << std::endl;
    }
}

/*
class WebSocketServer : public tcp::TcpServer::IEvent
{

public:

    virtual void NewConnection(const tcp::Conn &conn)
	{
        (void) conn;
	}

    virtual void ReadData(const tcp::Conn &conn)
	{
        std::cout << "Received: " << conn.payload << std::endl;

		WebSocketRequest req;
		req.Parse(data);

		if (req.IsValid())
		{
			std::string upgrade = req.Upgrade("tarotclub");
			std::cout << "Upgrade header: " << std::endl << upgrade << std::endl;
            tcp::TcpSocket peer(tcp::Peer(socket, false));
			peer.Send(upgrade);
		}
		else
		{
			std::cout << "Invalid request: " << std::endl << data << std::endl;
		}
	}

    virtual void ClientClosed(const tcp::Conn &conn)
	{
        (void) conn;
	}

    virtual void ServerTerminated(tcp::TcpServer::IEvent::CloseType type)
    {
        (void) type;
    }

};

*/

/*

// #define DEBUG_THREAD
// #define TEST_WEBSOCKET_HANDSHAKE

int main()
{
    std::thread clientThread;

	// Declare tcp server callbacks
	EchoServer echo;
//	WebSocketServer webSockEvent;

	// Actually instanciate the Tcp server thread using the previously defined callbacks
    tcp::TcpServer server(echo);
//    tcp::TcpServer webSock(webSockEvent);
    
	// Init the whole Tcp stack
    tcp::TcpSocket::Initialize();

	// Start the server threads
    server.Start(61617, 10U);
//	webSock.Start(4242, 5U);

#ifdef DEBUG_THREAD
    std::this_thread::sleep_for(std::chrono::seconds(2U));
    clientThread = std::thread(ClientThread);
    clientThread.join();
#endif


    server.Join();
	webSock.Join();

    return 0;
}

*/

TcpSocketTest::TcpSocketTest()
{

}

void TcpSocketTest::SimpleTcpEchoServer()
{
    EchoServer echo;
    std::thread client;

    tcp::TcpServer server(echo);

    // Init the whole Tcp stack
    tcp::TcpSocket::Initialize();

    server.Start(10U, false, 61617);

    // Then initialize the client
    std::this_thread::sleep_for(std::chrono::seconds(2U));
    client = std::thread(ClientThread);
    client.join();
    server.Stop();
    server.Join();

    QCOMPARE(echoTestSuccess, true);
}
