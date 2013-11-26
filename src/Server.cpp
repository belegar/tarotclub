/*=============================================================================
 * TarotClub - Server.cpp
 *=============================================================================
 * Host a Tarot game and manage network clients
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

#include "Server.h"
#include <QCoreApplication>


/*****************************************************************************/
Server::Server()
    : maximumPlayers(4)
    , tcpPort(DEFAULT_PORT)
    , enable(false)
{
    connect(&tcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    connect(&engine, &TarotEngine::sigEndOfTrick, this, &Server::slotSendWaitTrick);
    connect(&engine, &TarotEngine::sigStartDeal, this, &Server::slotSendStartDeal);
    connect(&engine, &TarotEngine::sigSendCards, this, &Server::slotSendCards);
    connect(&engine, &TarotEngine::sigPlayCard, this, &Server::slotSendPlayCard);
    connect(&engine, &TarotEngine::sigRequestBid, this, &Server::slotSendRequestBid);
    connect(&engine, &TarotEngine::sigShowDog, this, &Server::slotSendShowDog);
    connect(&engine, &TarotEngine::sigDealAgain, this, &Server::slotSendDealAgain);
    connect(&engine, &TarotEngine::sigEndOfDeal, this, &Server::slotSendEndOfDeal);
}
/*****************************************************************************/
void Server::SetMaximumPlayers(int n)
{
    maximumPlayers = n;
}
/*****************************************************************************/
void Server::slotNewConnection()
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
void Server::SetTcpPort(int port)
{
    tcpPort = port;
}
/*****************************************************************************/
int Server::GetTcpPort()
{
    return tcpPort;
}
/*****************************************************************************/
int Server::GetNumberOfConnectedPlayers()
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
TarotEngine &Server::GetEngine()
{
    return engine;
}
/*****************************************************************************/
void Server::NewServerGame(Game::Mode mode)
{
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

    // Init everything
    engine.NewGame(mode);
    emit sigServerMessage("Server started.\r\n");
}
/*****************************************************************************/

/**
 * @brief Properly eject clients and close the server
 */
void Server::StopServer()
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
}
/*****************************************************************************/
void Server::CloseClients()
{
    for (int i = 0; i < maximumPlayers; i++)
    {
        players[i].Close();
    }
}
/*****************************************************************************/
void Server::slotClientClosed(Place p)
{
    players[p].Close();
 //   SendChatMessage("The player " + engine.GetPlayer(p).GetIdentity().name + " has quit the game.");
 //   SendPlayersList();

    if (!enable)
    {
        disconnectedPlayers++;
    }
    // FIXME: if a player has quit during a game, replace it by a bot
}
/*****************************************************************************/
void Server::slotReadData(Place p)
{
    do
    {
        QByteArray data = players[p].GetData();
        if (enable)
        {
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
    }
    while (players[p].HasData());
}
/*****************************************************************************/
bool Server::DoAction(QDataStream &in, Place p)
{
    bool ret = true;

    // First byte is the command
    quint8 cmd;
    in >> cmd;

    switch (cmd)
    {
        case Protocol::CLIENT_MESSAGE:
        {
            QString message;
            in >> message;
            SendChatMessage(message);
            emit sigServerMessage(QString("Client message: ") + message);
            break;
        }

        case Protocol::CLIENT_INFOS:
        {
            Identity ident;
            in >> ident;

            ident.avatar = ":/images/avatars/" + ident.avatar;
            engine.GetPlayer(p).SetIdentity(ident);
            const QString m = "The player " + ident.name + " has joined the game.";
            SendChatMessage(m);
            emit sigServerMessage(m);

            if (GetNumberOfConnectedPlayers() == maximumPlayers)
            {
                SendPlayersList();
                engine.NewDeal();
            }
            break;
        }

        case Protocol::CLIENT_BID:
        {
            quint8 c;
            quint8 slam;
            // FIXME: add protection like client origin, current sequence...
            in >> c;
            in >> slam;

            Contract cont = engine.SetBid((Contract)c, (slam == 1 ? true : false), p);

            // Broadcast player's bid, and wait for all acknowlegements
            SendShowBid(cont, (slam == 1 ? true : false), p);
            break;
        }

        case Protocol::CLIENT_SYNC_DOG:
        {
            if (engine.SyncDog() == true)
            {
                SendBuildDiscard();
            }
            break;
        }

        case Protocol::CLIENT_DISCARD:
        {
            Deck discard;
            in >> discard;
            engine.SetDiscard(discard);
            break;
        }

        case Protocol::CLIENT_HANDLE:
        {
            // FIXME: add protections: handle validity, game sequence ...
            Deck handle;
            in >> handle;
            engine.SetHandle(handle, p);
            SendShowHandle(handle, p);
            break;
        }

        case Protocol::CLIENT_SYNC_START:
        {
            engine.SyncStart();
            break;
        }

        case Protocol::CLIENT_SYNC_BID:
        {
            engine.SyncBid();
            break;
        }

        case Protocol::CLIENT_SYNC_TRICK:
        {
            engine.SyncTrick();
            break;
        }

        case Protocol::CLIENT_SYNC_HANDLE:
        {
            engine.SyncHandle();
            break;
        }

        case Protocol::CLIENT_CARD:
        {
            quint8 id;
            Card *c;
            // TODO: tester la validité de la carte (ID + présence dans le jeu du joueur)
            // si erreur : logguer et prévenir quelqu'un ??
            // TODO: REFUSER SI MAUVAISE SEQUENCE EN COURS
            in >> id;
            c = TarotDeck::GetCard(id);
            engine.SetCard(c, p);

            // Broadcast played card, and wait for all acknowlegements
            SendShowCard(c);
            break;
        }

        case Protocol::CLIENT_SYNC_CARD:
        {
            engine.SyncCard();
            break;
        }

        case Protocol::CLIENT_READY:
        {
            engine.SyncReady();
            break;
        }

        default:
            TLogError("Unkown packet received");
            ret = false;
            break;
    }

    return ret;
}
/*****************************************************************************/
void Server::SendErrorServerFull(QTcpSocket *cnx)
{
    QByteArray packet;
    packet = BuildCommand(packet, Protocol::SERVER_ERROR_FULL);
    cnx->write(packet);
    cnx->flush();
}
/*****************************************************************************/
void Server::SendRequestIdentity(Place p)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::ReadWrite);
    out << (quint8)p; // assigned place
    out << (quint8)4; // number of players in the current game
    out << (quint8)engine.GetGameInfo().gameMode;
    packet = BuildCommand(packet, Protocol::SERVER_REQUEST_IDENTITY);
    players[p].SendData(packet);
}
/*****************************************************************************/
void Server::SendShowBid(Contract c, bool slam, Place p)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (quint8)p   // current player bid
        << (quint8)c;  // contract to show
    if (slam)
    {
        out << (quint8)1;
    }
    else
    {
        out << (quint8)0;
    }
    packet = BuildCommand(packet, Protocol::SERVER_SHOW_PLAYER_BID);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::SendChatMessage(const QString &message)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << message;
    packet = BuildCommand(packet, Protocol::SERVER_MESSAGE);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::SendDisconnect()
{
    QByteArray packet;
    packet = BuildCommand(packet, Protocol::SERVER_DISCONNECT);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::SendPlayersList()
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (quint8)GetNumberOfConnectedPlayers();
    for (int i = 0; i < maximumPlayers; i++)
    {
        if (players[i].IsFree() == false)
        {
            Identity ident = engine.GetPlayer((Place)i).GetIdentity();
            out << (quint8)i;
            out << ident;
        }
    }
    packet = BuildCommand(packet, Protocol::SERVER_PLAYERS_LIST);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::SendBuildDiscard()
{
    QByteArray packet;
    packet = BuildCommand(packet, Protocol::SERVER_BUILD_DISCARD);
    players[engine.GetGameInfo().taker].SendData(packet);
}
/*****************************************************************************/
void Server::SendShowCard(Card *c)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (quint8)c->GetId()
        << (quint8)engine.GetGameInfo().currentPlayer;
    packet = BuildCommand(packet, Protocol::SERVER_SHOW_CARD);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::SendShowHandle(Deck &handle, Place p)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (quint8)p;
    out << handle;
    packet = BuildCommand(packet, Protocol::SERVER_SHOW_HANDLE);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendCards()
{
    for (int i = 0; i < engine.GetGameInfo().numberOfPlayers; i++)
    {
        QByteArray packet;
        QDataStream out(&packet, QIODevice::WriteOnly);
        out << engine.GetPlayer((Place)i).GetDeck();
        packet = BuildCommand(packet, Protocol::SERVER_SEND_CARDS);
        players[i].SendData(packet);
    }
}
/*****************************************************************************/
void Server::slotSendEndOfDeal()
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << engine.GetScore();
    packet = BuildCommand(packet, Protocol::SERVER_END_OF_DEAL);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendWaitTrick(Place winner)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (quint8)winner;
    packet = BuildCommand(packet, Protocol::SERVER_END_OF_TRICK);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendStartDeal()
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (quint8)engine.GetGameInfo().taker
        << (quint8)engine.GetGameInfo().contract;
    packet = BuildCommand(packet, Protocol::SERVER_START_DEAL);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendPlayCard(Place p)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (quint8)p; // this player has to play a card
    packet = BuildCommand(packet, Protocol::SERVER_PLAY_CARD);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendRequestBid(Contract c, Place p)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (quint8)c; // previous bid
    out << (quint8)p; // player to declare something
    packet = BuildCommand(packet, Protocol::SERVER_REQUEST_BID);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendShowDog()
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << engine.GetDeal().GetDog();
    packet = BuildCommand(packet, Protocol::SERVER_SHOW_DOG);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendDealAgain()
{
    QByteArray packet;
    packet = BuildCommand(packet, Protocol::SERVER_DEAL_AGAIN);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::Broadcast(QByteArray &block)
{
    for (int i = 0; i < maximumPlayers; i++)
    {
        players[i].SendData(block);
        // Calm down packets sending
        qApp->processEvents(QEventLoop::AllEvents, 100);
    }
}

//=============================================================================
// End of file Server.cpp
//=============================================================================


