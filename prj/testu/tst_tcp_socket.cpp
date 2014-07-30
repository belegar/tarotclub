#include <iostream>
#include <thread>
#include <chrono>

#include "TcpClient.h"
#include "TcpServer.h"
#include "WebSocket.h"


/**
 * @brief Simple echo server
 */
class EchoServer : public TcpServer::IEvent
{

public:

	virtual void NewConnection(int socket)
	{
	
	}

	virtual void ReadData(int socket, const std::string &data)
	{
		// echo the data received
		TcpSocket peer;
		peer.SetSocket(socket);
		peer.Send(data);
	}

	virtual void ClientClosed(int socket)
	{
	}

};

void ClientThread()
{
    TcpClient client;
    std::string buff;

    if (client.Start())
    {
        client.Connect("127.0.0.1", 61617);

        for (;;)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1U));
            if (client.Send("Hello, world!"))
            {
                if (client.Recv(buff))
                {
                    std::cout << "Received: " << buff << std::endl;
                }
                else
                {
                    std::cout << "Read error" << std::endl;
                    break;
                }
            }
            else
            {
                std::cout << "Write error" << std::endl;
                break;
            }
        }
    }
    std::cout << "Exiting client thread..." << std::endl;
}


class WebSocketServer : public TcpServer::IEvent
{

public:

	virtual void NewConnection(int socket)
	{

	}

	virtual void ReadData(int socket, const std::string &data)
	{
		std::cout << "Received: " << data << std::endl;

		WebSocketRequest req;
		req.Parse(data);

		if (req.IsValid())
		{
			std::string upgrade = req.Upgrade("tarotclub");
			std::cout << "Upgrade header: " << std::endl << upgrade << std::endl;
			TcpSocket peer;
			peer.SetSocket(socket);
			peer.Send(upgrade);
		}
		else
		{
			std::cout << "Invalid request: " << std::endl << data << std::endl;
		}
	}

	virtual void ClientClosed(int socket)
	{
	}

};


// #define DEBUG_THREAD
// #define TEST_WEBSOCKET_HANDSHAKE

int main()
{

#ifdef TEST_WEBSOCKET_HANDSHAKE

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
			std::cout << "Invalid accept encoded string: " << accept << std::endl;
		}
	}
	else
	{
		std::cout << "Invalid request" << std::endl;
	}
#endif
    std::thread clientThread;

	// Declare tcp server callbacks
	EchoServer echo;
	WebSocketServer webSockEvent;

	// Actually instanciate the Tcp server thread using the previously defined callbacks
    TcpServer server(echo);
	TcpServer webSock(webSockEvent);
    
	// Init the whole Tcp stack
    TcpSocket::Initialize();

	// Start the server threads
    server.Start(61617, 10U);
	webSock.Start(4242, 5U);

#ifdef DEBUG_THREAD
    std::this_thread::sleep_for(std::chrono::seconds(2U));
    clientThread = std::thread(ClientThread);
    clientThread.join();
#endif


    server.Join();
	webSock.Join();

    return 0;
}

