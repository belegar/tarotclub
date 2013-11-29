/*=============================================================================
 * TarotClub - Table.cpp
 *=============================================================================
 * The table manages the Tarot thread server
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */
#include "Table.h"

/*****************************************************************************/
Table::Table()
    : maximumPlayers(4)
    , tcpPort(DEFAULT_PORT)
{
    connect(&tcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}
/*****************************************************************************/
void Table::slotNewConnection()
{
    while (tcpServer.hasPendingConnections())
    {
        QTcpSocket *cnx = tcpServer.nextPendingConnection();
        if (GetNumberOfConnectedPlayers() == maximumPlayers)
        {
            SendErrorServerFull(cnx);
        }
        else
        {
            Place p = NOWHERE;

            // Look for free space
            for (int i = 0; i < maximumPlayers; i++)
            {
                if (players[i].IsFree() == true)
                {
                    p = (Place) i;
                    break;
                }
            }

            if (p != NOWHERE)
            {
                players[p].SetConnection(cnx, p);
                connect(&players[p], SIGNAL(sigDisconnected(Place)), this, SLOT(slotClientClosed(Place)));
                connect(&players[p], SIGNAL(sigReadyRead(Place)), this, SLOT(slotReadData(Place)));
                SendRequestIdentity(p);
            }
            else
            {
                TLogError("Error, cannot find any free place.");
            }
        }
    }
}
/*****************************************************************************/
void Table::StopServer()
{
    CloseClients();
    tcpServer.close();
}
/*****************************************************************************/
void Table::CloseClients()
{
    for (int i = 0; i < maximumPlayers; i++)
    {
        players[i].Close();
    }
}
/*****************************************************************************/
void Table::slotClientClosed(Place p)
{
    players[p].Close();
    SendChatMessage("The player " + engine.GetPlayer(p).GetIdentity().name + " has quit the game.");
    SendPlayersList();

    if (!enable)
    {
        disconnectedPlayers++;
    }

    // FIXME: if a player has quit during a game, replace it by a bot
}
/*****************************************************************************/
void Table::slotReadData(Place p)
{
    do
    {
        QByteArray data = players[p].GetData();
        QDataStream in(data);
        if (DecodePacket(in) == true)
        {
            if (DoAction(in, p) == false)
            {
                // bad packet received, exit decoding
                return;
            }
        }
    }
    while (players[p].HasData());
}
/*****************************************************************************/
void Table::Broadcast(QByteArray &block)
{
    for (int i = 0; i < maximumPlayers; i++)
    {
        players[i].SendData(block);
        // Calm down packets sending
        qApp->processEvents(QEventLoop::AllEvents, 100);
    }
}
/*****************************************************************************/
int Table::GetNumberOfConnectedPlayers()
{
    int p = 0;
    for (int i = 0; i < maximumPlayers; i++)
    {
        if (players[i].IsFree() == false)
        {
            p++;
        }
    }
    return p;
}
/*****************************************************************************/
void Table::LoadConfiguration(int port)
{
    serverConfig.Load();

    // Apply configuration
    for (int i = 0; i < 3; i++)
    {
        bots[i].SetMyIdentity(serverConfig.GetOptions().bots[i]);
        bots[i].SetTimeBeforeSend(serverConfig.GetOptions().timer);
    }

    tcpPort = tcpPort;
}
/*****************************************************************************/
void Table::SaveConfiguration(const ServerOptions &opt)
{
    serverConfig.SetOptions(opt);
    serverConfig.Save();
}
/*****************************************************************************/
void Table::CreateGame(Game::Mode gameMode, int nbPlayers)
{
    // TODO: add support for 3 and 5 players game
    if (nbPlayers != 4)
    {
        return;
    }
    maximumPlayers = nbPlayers;

    StopServer();

    if (tcpServer.isListening())
    {
        tcpServer.resumeAccepting();
    }
    else
    {
        // Add few players to the maximum allowed to manage pending connections
        tcpServer.setMaxPendingConnections(maximumPlayers + 3);
        tcpServer.listen(QHostAddress::LocalHost, tcpPort);
    }
    enable = true;

    // Add few players to the maximum allowed to manage pending connections
    tcpServer.setMaxPendingConnections(maximumPlayers + 3);
    tcpServer.listen(QHostAddress::LocalHost, tcpPort);

    server.NewServerGame(gameMode);
}
/*****************************************************************************/
void Table::Stop()
{
    // 1. Disable network protocol
        enable = false;

        if (tcpServer.isListening())
        {
            // 2. Don't accept any new connections
            tcpServer.pauseAccepting();

            // 3. Send a disconnection message to all the connected players
            disconnectedPlayers = 0;
            SendDisconnect();

            int timeout = 20;
            while (timeout)
            {
                qApp->processEvents(QEventLoop::AllEvents, 100);
                if (disconnectedPlayers >= maximumPlayers)
                {
                    break;
                }
                timeout--;
            }

            // 5. Close sokets
            //CloseClients();
        }

    server.StopServer();
}
/*****************************************************************************/
Server &Table::GetServer()
{
    return server;
}
/*****************************************************************************/
ServerOptions &Table::GetOptions()
{
    return serverConfig.GetOptions();
}
/*****************************************************************************/
TarotEngine::Shuffle Table::GetShuffle()
{
    return server.GetEngine().GetShuffle();
}
/*****************************************************************************/
void Table::SetShuffle(const TarotEngine::Shuffle &s)
{
    server.GetEngine().SetShuffle(s);
}
/*****************************************************************************/
void Table::ConnectBots()
{
    int i;

    qApp->processEvents(QEventLoop::AllEvents, 100);
    for (i = 0; i < 3; i++)
    {
        bots[i].Initialize();
        bots[i].ConnectToHost("127.0.0.1", GetOptions().port);
        qApp->processEvents(QEventLoop::AllEvents, 100);
    }
}

//=============================================================================
// End of file Table.cpp
//=============================================================================

