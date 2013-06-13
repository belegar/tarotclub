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
    : tcpPort(DEFAULT_PORT)
{
    connect(&tcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    connect(&engine, &TarotEngine::sigEndOfTrick, this, &Server::slotSendWaitTrick);
    connect(&engine, &TarotEngine::sigStartDeal, this, &Server::slotSendStartDeal);
    connect(&engine, &TarotEngine::sigSelectPlayer, this, &Server::slotSendSelectPlayer);
    connect(&engine, &TarotEngine::sigPlayCard, this, &Server::slotSendStartDeal);
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
    while(tcpServer.hasPendingConnections())
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
            for (int i=0; i<maximumPlayers; i++)
            {
                if (players[i].IsFree() == true)
                {
                    p = (Place) p;
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
                qDebug() << "Error, cannot find any free place." << endl;
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
int Server::GetNumberOfConnectedPlayers()
{
    int p = 0;
    for (int i=0; i<maximumPlayers; i++)
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
void Server::NewServerGame(TarotEngine::GameMode mode)
{
    int port;

    StopServer();
    tcpServer.setMaxPendingConnections(maximumPlayers + 3); // Add few players to the maximum for clients trying to access

    if (mode == TarotEngine::NET_GAME_SERVER)
    {
        port = options.port;
    }
    else
    {
        // if local game, always use the default port
        port = DEFAULT_PORT;
    }
    tcpServer.listen(QHostAddress::LocalHost, port);

    // Init everything
    engine.NewGame(mode);
    emit sigServerMessage("Server started.\r\n");
}
/*****************************************************************************/
void Server::StopServer()
{
    CloseClients();
    tcpServer.close();
}
/*****************************************************************************/
void Server::CloseClients()
{
    for (int i=0; i<maximumPlayers; i++)
    {
        players[i].Close();
    }
}
/*****************************************************************************/
void Server::slotClientClosed(Place p)
{
    players[p].Close();
    SendChatMessage( "The player " + players[p].GetIdentity().name + " has quit the game.");
    SendPlayersList();

    // FIXME: if a player has quit during a game, replace it by a bot
}
/*****************************************************************************/
void Server::slotReadData(Place p)
{
    QByteArray data = players[p].GetData();
    QDataStream in(data);

    while (DecodePacket(in, data.size()) == true)
    {
        DoAction(in, p);
    }
}
/*****************************************************************************/
void Server::DoAction(QDataStream &in, Place p)
{
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
        players[p].SetIdentity(ident);
        const QString m = "The player " + ident.name + " has joined the game.";
        SendChatMessage(m);
        emit sigServerMessage(m);
        SendPlayersList();

        if (GetNumberOfConnectedPlayers() == maximumPlayers)
        {
            engine.NewDeal();
        }
        break;
    }

    case Protocol::CLIENT_BID:
    {
        quint8 c;
        // FIXME: add protection like client origin, current sequence...
        in >> c;
        engine.BidSequence((Contract)c, p);
        SendBid((Contract)c, p);
        break;
    }

    case Protocol::CLIENT_SYNC_DOG:
        if (engine.SyncDog() == true)
        {
            SendBuildDiscard();
        }
        break;

    case Protocol::CLIENT_DISCARD:
    {
        Deck discard;
        in >> discard;
        engine.SetDiscard(discard);
        break;
    }

    case Protocol::CLIENT_HANDLE:
    {
        // FIXME: add protection, limits ...
        quint8 size;
        Deck handle;
        in >> size; // FIXME: delete this unused variable?
        in >> handle;
        engine.SetHandle(handle, p);
        SendShowHandle(handle);
        break;
    }

    case Protocol::CLIENT_SYNC_TRICK:
    {
        engine.SyncTrick();
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
        SendShowCard(c);
        break;
    }

    case Protocol::CLIENT_READY:
        engine.SyncReady();
        break;

    default:
        break;
    }
}
/*****************************************************************************/
void Server::SendRequestIdentity(Place p)
{
    QDataStream out;
    out << (quint8)p; // assigned place
    QByteArray packet = BuildCommand(out, Protocol::SERVER_REQUEST_IDENTITY);
    players[p].SendData(packet);
}
/*****************************************************************************/
void Server::SendCards()
{
    for (int i=0; i<engine.GetGameInfo().numberOfPlayers; i++)
    {
        QDataStream out;
        out << (quint8)4 // number of players in the current game
            << engine.GetPlayer((Place)i).GetDeck();
        QByteArray packet = BuildCommand(out, Protocol::SERVER_SEND_CARDS);
        players[i].SendData(packet);
    }
}
/*****************************************************************************/
void Server::SendBid(Contract c, Place p)
{
    QDataStream out;
    out << (quint8)p   // current player bid
        << (quint8)c;  // contract to show
    QByteArray packet = BuildCommand(out, Protocol::SERVER_SHOW_PLAYER_BID);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::SendChatMessage(const QString &message)
{
    QDataStream out;
    out << message;
    QByteArray packet = BuildCommand(out, Protocol::SERVER_MESSAGE);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::SendPlayersList()
{
    QDataStream out;
    out << (quint8)GetNumberOfConnectedPlayers();
    for (int i=0; i<maximumPlayers; i++)
    {
        if (players[i].IsFree() == false)
        {
            out << players[i].GetPlace()
                << players[i].GetIdentity();
        }
    }
    QByteArray packet = BuildCommand(out, Protocol::SERVER_PLAYERS_LIST);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::SendBuildDiscard()
{
    QDataStream out;
    QByteArray packet = BuildCommand(out, Protocol::SERVER_BUILD_DISCARD);
    players[engine.GetGameInfo().taker].SendData(packet);
}
/*****************************************************************************/
void Server::SendShowCard(Card *c)
{
    QDataStream out;
    out << (quint8)c->GetId()
        << (quint8)engine.GetGameInfo().currentPlayer;
    QByteArray packet = BuildCommand(out, Protocol::SERVER_SHOW_CARD);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::SendShowHandle(Deck &handle)
{
    QDataStream out;
    out << handle;
    QByteArray packet = BuildCommand(out, Protocol::SERVER_SHOW_HANDLE);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendEndOfDeal()
{
    QDataStream out;
    out << engine.GetScore();
    QByteArray packet = BuildCommand(out, Protocol::SERVER_END_OF_DEAL);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendWaitTrick(Place winner)
{
    QDataStream out;
    out << (quint8)winner;
    QByteArray packet = BuildCommand(out, Protocol::SERVER_END_OF_TRICK);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendStartDeal()
{
    QDataStream out;
    out << (quint8)engine.GetGameInfo().taker
        << (quint8)engine.GetGameInfo().contract;
    QByteArray packet = BuildCommand(out, Protocol::SERVER_START_DEAL);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendSelectPlayer(Place p)
{
    QDataStream out;
    out << (quint8)p;
    QByteArray packet = BuildCommand(out, Protocol::SERVER_SELECT_PLAYER);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendPlayCard(Place p)
{
    QDataStream out;
    out << (quint8)p;
    QByteArray packet = BuildCommand(out, Protocol::SERVER_PLAY_CARD);
    players[p].SendData(packet);
}
/*****************************************************************************/
void Server::slotSendRequestBid(Contract c, Place p)
{
    QDataStream out;
    out << (quint8)c; // previous bid
    QByteArray packet = BuildCommand(out, Protocol::SERVER_REQUEST_BID);
    players[p].SendData(packet);
}
/*****************************************************************************/
void Server::slotSendShowDog()
{
    QDataStream out;
    out << engine.GetDeal().GetDog();
    QByteArray packet = BuildCommand(out, Protocol::SERVER_SHOW_DOG);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::slotSendDealAgain()
{
    QDataStream out;
    QByteArray packet = BuildCommand(out, Protocol::SERVER_DEAL_AGAIN);
    Broadcast(packet);
}
/*****************************************************************************/
void Server::Broadcast(QByteArray &block)
{
    for (int i=0; i<maximumPlayers; i++)
    {
        players[i].SendData(block);
    }
}



//=============================================================================
// End of file Server.cpp
//=============================================================================


