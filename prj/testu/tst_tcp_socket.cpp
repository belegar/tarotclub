#include <iostream>
#include <thread>
#include <chrono>

#include "TcpClient.h"
#include "TcpServer.h"

using namespace std;

/**
 * @brief main
 * @return
 *
 * TODO:
  * - Manage Error message and code, add TcpServer error strings during initialization
 */

class EchoServer : public Observer<TcpServer::Signal>
{

public:
    void Update(const TcpServer::Signal &sig)
    {
        if (sig.type == TcpServer::DATA_RECEIVED)
        {
            TcpSocket socket;
            socket.SetSocket(sig.socket);
            socket.Send(sig.data);
        }
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

int main()
{
    std::thread clientThread;
    TcpServer server;
    EchoServer echo;

    TcpSocket::Initialize();

    server.RegisterListener(echo);
    server.Start(61617, 10U);

    std::this_thread::sleep_for(std::chrono::seconds(2U));

    clientThread = std::thread(ClientThread);

    clientThread.join();
    server.Join();

    return 0;
}

