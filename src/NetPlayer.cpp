/*=============================================================================
 * TarotClub - NetPlayer.cpp
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

#include "NetPlayer.h"

/*****************************************************************************/
NetPlayer::NetPlayer()
    : socket(NULL)
    , freePlace(true)
{

}
/*****************************************************************************/
bool NetPlayer::IsFree()
{
    return freePlace;
}
/*****************************************************************************/
void NetPlayer::Close()
{
    if (socket != NULL)
    {
        QObject::connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
        socket->close();
        socket = NULL;
    }
    freePlace = true;
}
/*****************************************************************************/
bool NetPlayer::HasData()
{
    if (socket->bytesAvailable())
    {
        return true;
    }
    else
    {
        return false;
    }
}
/*****************************************************************************/
Identity &NetPlayer::GetIdentity()
{
    return player.GetIdentity();
}
/*****************************************************************************/
void NetPlayer::SetConnection(QTcpSocket *s, Place p)
{
    socket = s;
    freePlace = false;
    player.SetPlace(p);
    connect(socket, &QTcpSocket::disconnected, this, &NetPlayer::slotClientClosed);
    connect(socket, &QTcpSocket::readyRead, this, &NetPlayer::slotReadData);
}
/*****************************************************************************/
void NetPlayer::SetIdentity(const Identity &ident)
{
    player.SetIdentity(ident);
}
/*****************************************************************************/
void NetPlayer::SendData(QByteArray &data)
{
    if (socket != NULL)
    {
        if (socket->isValid())
        {
            socket->write(data);
            socket->flush();
        }
        else
        {
            qDebug() << "Send data to a non-connected socket" << endl;
        }
    }
}
/*****************************************************************************/
QByteArray NetPlayer::GetData()
{
    QByteArray data;
    if (socket != NULL)
    {
        if (socket->isValid())
        {
            data = socket->readAll();
        }
    }
    return data;
}
/*****************************************************************************/
Place NetPlayer::GetPlace()
{
    return player.GetPlace();
}
/*****************************************************************************/
void NetPlayer::slotClientClosed()
{
    emit sigDisconnected(player.GetPlace());
}
/*****************************************************************************/
void NetPlayer::slotReadData()
{
    emit sigReadyRead(player.GetPlace());
}


//=============================================================================
// End of file Netlayer.cpp
//=============================================================================
