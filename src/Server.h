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
#include "NetPlayer.h"
#include "Protocol.h"
#include "TarotEngine.h"
#include "Bot.h"
#include "ServerConfig.h"

/*****************************************************************************/
class Server : public Observer<TarotEngine::SignalInfo>
{

public:
    Server();

    // Helpers
    void NewServerGame(Game::Mode mode);
    void StopServer();
    void ConnectBots();

    // Getters
    QList<Identity> GetPlayers();
    int GetNumberOfConnectedPlayers();
    TarotEngine &GetEngine();
    int GetTcpPort();

    // Setters
    void SetMaximumPlayers(int n);
    void SetTcpPort(int port);

signals:
    void sigServerMessage(QString const &);

private:
    int maximumPlayers;
    int size;
    NetPlayer players[5]; // [3..5] players
    QTcpServer tcpServer;
    TarotEngine engine;
    int tcpPort;
    bool enable;
    int disconnectedPlayers;


    void Update(const TarotEngine::SignalInfo &info);
    void CloseClients();
    bool DoAction(QDataStream &in, Place p);

    /**
     * @brief Broadcast a packet to connected players
     * @param block
     */
    void Broadcast(QByteArray &block);

    // Protocol methods
    void SendRequestIdentity(Place p);
    void SendDisconnect();
    void SendShowBid(Contract c, bool slam, Place p);
    void SendErrorServerFull(QTcpSocket *cnx);
    void SendChatMessage(const QString &message);
    void SendPlayersList();
    void SendBuildDiscard();
    void SendShowCard(Card *c);
    void SendShowHandle(Deck &handle, Place p);


    // TarotClub protocol
    void slotSendCards();
    void slotNewConnection();
    void slotSendWaitTrick(Place winner);
    void slotSendStartDeal();
    void slotSendPlayCard(Place p);
    void slotSendRequestBid(Contract c, Place p);
    void slotSendShowDog();
    void slotSendDealAgain();
    void slotSendEndOfDeal();

    // client sockets
    void slotClientClosed(Place p);
    void slotReadData(Place p);
};

#endif // SERVER_H

//=============================================================================
// End of file Server.h
//=============================================================================
