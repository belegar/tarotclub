/*=============================================================================
 * TarotClub - NetPlayer.h
 *=============================================================================
 * Networked remote player, used in the server side
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

#ifndef _NET_PLAYER_H
#define _NET_PLAYER_H

#include <QtNetwork>
#include "defines.h"
#include "Deck.h"
#include "Identity.h"
#include "Player.h"

/*****************************************************************************/
class NetPlayer : public QObject
{
    Q_OBJECT

public:
    NetPlayer();

    // Helpers
    bool IsFree();
    void SendData(QByteArray &data);
    void Close();

    // Getters
    Identity &GetIdentity();
    QTcpSocket *GetSocket();
    QByteArray GetData();
    Place GetPlace();

    // Setters
    void SetConnection(QTcpSocket *s, Place p);
    void SetIdentity(const Identity &ident);

signals:
    void sigDisconnected(Place);
    void sigReadyRead(Place);

private:
    QTcpSocket *socket;
    bool freePlace;
    Player player;

private slots:
    void slotClientClosed();
    void slotReadData();

};

#endif // _NET_PLAYER_H

//=============================================================================
// End of file NetPlayer.h
//=============================================================================
