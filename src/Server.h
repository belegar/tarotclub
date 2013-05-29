/*=============================================================================
 * TarotClub - Server.h
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
#ifndef SERVER_H
#define SERVER_H

#include <QtNetwork>
#include "Player.h"
#include "Protocol.h"
#include "TarotEngine.h"
#include "Bot.h"
#include "ServerConfig.h"

/*****************************************************************************/
class NetPlayer
{
public:
    NetPlayer()
        : freePlace(true)
    {

    }
    QTcpSocket  socket;
    bool    freePlace;
};
/*****************************************************************************/
class Server : public Protocol
{
public:
    TarotServer();

    // Server management
    QList<Identity> GetPlayers();
    int GetNumberOfPlayers();
    QTcpSocket *GetConnection(Place p);
    void SetMaximumPlayers(int n);

signals:
    void sigPrintMessage(const QString &);

public slots:
    void slotClientClosed();

    /**
     * @brief Decode received network packet
     */
    void slotReadData();

private:

    int maximumPlayers;
    int size;
    NetPlayer players[5]; // [3..5] players
    QTcpServer tcpServer;
    TarotEngine engine;
    Bot         bots[3];       // the computer
    ServerOptions  options;

    void CloseClients();

    /**
     * @brief Implementation of data management
     */
    void DoAction(QDataStream &in);

    /**
     * @brief Broadcast a packet to connected players
     * @param block
     */
    void Broadcast(QByteArray &block);

    // Protocol methods
    void SendAskIdentity(QTcpSocket *cnx, Place p);
    void SendCards(Place p, quint8 *params);
    void SendAskBid(Contrat c);
    void SendBid(Place p, Contrat c);
    void SendErrorServerFull(QTcpSocket *cnx);
    void SendMessage(const QString &message, Place p);
    void SendPlayersList();
    void SendShowChien();
    void SendDoChien();
    void SendJoueCarte();
    void SendCard(Card *c);
    void SendDepartDonne();
    void SendRedist();
    void SendFinDonne(ScoreInfo &score_inf, bool lastDeal, float pointsTour);
    void SendWaitPli(float pointsTour);
    void SendSelectPlayer(Place p);

};

#endif // SERVER_H

//=============================================================================
// End of file Server.h
//=============================================================================
