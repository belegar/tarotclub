/*=============================================================================
 * TarotClub - Lobby.cpp
 *=============================================================================
 * Manage temporary connections to join free game tables
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 *=============================================================================
 */

#include "Lobby.h"
#include "Log.h"
#include "Util.h"
#include <sstream>
#include <vector>

/*****************************************************************************/
Lobby::Lobby()
    : mTcpServer(*this)
{
    saloons[0].name = "Noobs";
    saloons[1].name = "Normal";

    for (int j = 0; j < SERVER_MAX_SALOONS; j++)
    {
        for (int i = 0; i < SERVER_MAX_TABLES; i++)
        {
            std::stringstream ss;
            ss << (i + 1);
            saloons[j].tables[i].name = "Table " + ss.str();
        }
    }
}
/*****************************************************************************/
Lobby::~Lobby()
{
    mTcpServer.Close();
}
/*****************************************************************************/
void Lobby::Initialize()
{
    TarotDeck::Initialize();

    Tarot::Shuffle sh;
    sh.type = Tarot::Shuffle::RANDOM_DEAL;
    int tcpPort = 33000; // FIXME: make this as an option

    for (int j = 0; j < SERVER_MAX_SALOONS; j++)
    {
        for (int i = 0; i < SERVER_MAX_TABLES; i++)
        {
            saloons[j].tables[i].table.SetTcpPort(tcpPort);
            saloons[j].tables[i].table.Initialize(); // Start all threads and TCP sockets
            saloons[j].tables[i].table.CreateGame(Tarot::ONE_DEAL, 4U, sh);
            // Each table has a unique port
            tcpPort++;
        }
    }

    // Lobby TCP server
    mTcpServer.Start(4242, 20U);
}
/*****************************************************************************/
void Lobby::WaitForEnd()
{
    mTcpServer.Join();
}
/*****************************************************************************/
void Lobby::ClientClosed(int socket)
{
    (void)socket;
}
/*****************************************************************************/
void Lobby::ReadData(int socket, const std::string &data)
{
    // This slot is called when the client sent data to the server. The
    // server looks if it was a get request and sends a very simple ASCII

    // Remove trailing \n
    std::string command = data;
    command.pop_back();



    std::vector<std::string> tokens = Util::Split(command, ":");
    std::stringstream ss;


    std::string request = data.substr(0, 3);
    if (request == "GET")
    {
        std::cout << data << std::endl;

        ss << "HTTP/1.1 101 Switching Protocols"
              "Upgrade: websocket"
              "Connection: Upgrade"
              "Access-Control-Allow-Origin: http://example.com"
              "Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo="
              "Sec-WebSocket-Protocol: appProtocol-v2"
    }

    if (tokens[0] == "GET")
    {
        if (tokens[1] == "INFOS")
        {
            std::vector<std::string> list;

            for (int i = 0; i < SERVER_MAX_SALOONS; i++)
            {
                list.push_back(saloons[i].name);
            }

            ss << "SALOON:";
            ss << Util::Join(list, ",");
            ss << "\n";
        }
        else if (tokens[1] == "TABLES")
        {
            std::vector<std::string> list;

            for (int i = 0; i < SERVER_MAX_SALOONS; i++)
            {
                if (saloons[i].name == tokens[2])
                {
                    for (int j = 0; j < SERVER_MAX_TABLES; j++)
                    {
                        list.push_back(saloons[i].tables[j].name);
                    }
                }
            }

            ss << "TABLES:";
            ss << Util::Join(list, ",");
            ss << "\n";
        }
        else if (tokens[1] == "PORT")
        {
            std::vector<std::string> param = Util::Split(tokens[2], ",");

            for (int i = 0; i < SERVER_MAX_SALOONS; i++)
            {
                if (saloons[i].name == param[0])
                {
                    for (int j = 0; j < SERVER_MAX_TABLES; j++)
                    {
                        if (saloons[i].tables[j].name == param[1])
                        {
                            ss << "PORT:";
                            ss << saloons[i].tables[j].table.GetTcpPort();
                            ss << "\n";
                        }
                    }
                }
            }
        }
    }
    TcpSocket sock(socket);

    // If there is something to reply, do it
    if (ss.str().size() > 0)
    {
        sock.Send(ss.str());
    }
    else
    {
        // Or reply an error
        sock.Send("ERROR");
    }
}
/*****************************************************************************/
void Lobby::NewConnection(int socket)
{
    (void)socket;
}

//=============================================================================
// End of file Lobby.cpp
//=============================================================================
