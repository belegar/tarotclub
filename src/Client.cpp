/*=============================================================================
 * TarotClub - Client.cpp
 *=============================================================================
 * This class manages the network protocol and is used to join a online game
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

#ifndef QT_NO_DEBUG
#include <iostream>
#include <fstream>
using namespace std;
#endif // QT_NO_DEBUG

#include "Client.h"
#include "Identity.h"
#include "defines.h"

/*****************************************************************************/
Client::Client()
{
    // Events on the socket
    connect(&socket, SIGNAL(readyRead()), this, SLOT(slotSocketReadData()));
    connect(&socket, SIGNAL(disconnected()), this, SLOT(slotSocketClosed()));
    connect(&socket, SIGNAL(connected()), this, SLOT(slotSocketConnected()));
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotSocketError(QAbstractSocket::SocketError)));
    connect(&socket, SIGNAL(hostFound()), this, SLOT(slotSocketHostFound()));
}
/*****************************************************************************/
void Client::Initialize()
{
    score.Reset();
}
/*****************************************************************************/
Deck::Statistics &Client::GetStatistics()
{
    return stats;
}
/*****************************************************************************/
Deck &Client::GetCurrentTrick()
{
    return currentTrick;
}
/*****************************************************************************/
Deck &Client::GetDogDeck()
{
    return dogDeck;
}
/*****************************************************************************/
Deck &Client::GetHandleDeck()
{
    return handleDeck;
}
/*****************************************************************************/
Deck &Client::GetMyDeck()
{
    return player.GetDeck();
}
/*****************************************************************************/
Identity &Client::GetMyIdentity()
{
    return player.GetIdentity();
}
/*****************************************************************************/
bool Client::TestHandle()
{
    player.GetDeck().AnalyzeTrumps(stats);
    if ((handleDeck.HasFool() == true) && (stats.trumps > handleDeck.size()))
    {
        return false;
    }
    else
    {
        return true;
    }
}
/*****************************************************************************/
Game &Client::GetGameInfo()
{
    return info;
}
/*****************************************************************************/
Score &Client::GetScore()
{
    return score;
}
/*****************************************************************************/
void Client::SetMyIdentity(const Identity &ident)
{
    player.SetIdentity(ident);
}
/*****************************************************************************/
Contract Client::CalculateBid()
{
    int total = 0;
    Contract cont;

    UpdateStatistics();

    // Set points according to the card values
    if (stats.vingtEtUn == true)
    {
        total += 9;
    }
    if (stats.excuse == true)
    {
        total += 7;
    }
    if (stats.petit == true)
    {
        if (stats.trumps == 5)
        {
            total += 5;
        }
        else if (stats.trumps == 6 || stats.trumps == 7)
        {
            total += 7;
        }
        else if (stats.trumps > 7)
        {
            total += 8;
        }
    }

    // Each trump is 1 point
    // Each major trump is 1 more point
    total += stats.trumps * 2;
    total += stats.atoutsMajeurs * 2;
    total += stats.rois * 6;
    total += stats.dames * 3;
    total += stats.cavaliers * 2;
    total += stats.valets;
    total += stats.mariages;
    total += stats.longues * 5;
    total += stats.coupes * 5;
    total += stats.singletons * 3;
    total += stats.sequences * 4;

    // We can decide the bid
    if (total <= 35)
    {
        cont = PASS;
    }
    else if (total >= 36  && total <= 50)
    {
        cont = TAKE;
    }
    else if (total >= 51  && total <= 65)
    {
        cont = GUARD;
    }
    else if (total >= 66  && total <= 75)
    {
        cont = GUARD_WITHOUT;
    }
    else
    {
        cont = GUARD_AGAINST;
    }
    return cont;
}
/*****************************************************************************/
void Client::UpdateStatistics()
{
    player.GetDeck().AnalyzeTrumps(stats);
    player.GetDeck().AnalyzeSuits(stats);
}
/*****************************************************************************/
void Client::BuildDogDeck()
{
    int i;
    Card *c, *cdeck;

#ifndef QT_NO_DEBUG
    // Dog before
    ofstream f("dog_before.txt");
    f << dogDeck.GetCardList().toStdString() << endl;
    f.close();
#endif

    bool ok = false;
    i = 0;

    // on cherche si il y a un atout, une excuse, ou un roi dans le chien
    // et on les remplace par des cartes valides

    // We're looking for trumps or kings in the deck and we replace
    // them by other valid cards
    while (ok == false)
    {
        c = dogDeck.at(i);
        if ((c->GetSuit() == Card::TRUMPS) ||
                ((c->GetSuit() != Card::TRUMPS) && (c->GetValue() == 14)))
        {
            // looking for valid card
            int k = player.GetDeck().size();
            for (int j = 0; j < k; j++)
            {
                cdeck = player.GetDeck().at(j);
                if ((cdeck->GetSuit() != Card::TRUMPS) && (cdeck->GetValue() < 14))
                {
                    // Swap cards
                    player.GetDeck().removeAll(cdeck);
                    player.GetDeck().append(c);
                    dogDeck.removeAll(c);
                    dogDeck.append(cdeck);
                    break;
                }
            }
            i = 0;
        }
        else
        {
            i++;
        }

        if (i == 6)
        {
            ok = true;
        }
    }

#ifndef QT_NO_DEBUG
    // Dog after
    f.open("dog_after.txt");
    f << dogDeck.GetCardList().toStdString() << endl;
    f.close();
#endif
}
/*****************************************************************************/
Card *Client::Play()
{
    Card *c = NULL;

    for (int i = 0; i < player.GetDeck().size(); i++)
    {
        c = player.GetDeck().at(i);
        if (IsValid(c) == true)
        {
            break;
        }
    }
    return c;
}
/*****************************************************************************/
bool Client::IsValid(Card *c)
{
    return player.CanPlayCard(c, currentTrick, info);
}
/*****************************************************************************/
void Client::ConnectToHost(const QString &hostName, quint16 port)
{
    socket.connectToHost(hostName, port);
}
/*****************************************************************************/
void Client::Close()
{
    socket.close();
}
/*****************************************************************************/
void Client::slotSocketConnected()
{
    QString msg = player.GetIdentity().name + trUtf8(" is connected.");
    qDebug() << msg.toLatin1().constData();
}
/*****************************************************************************/
void Client::slotSocketHostFound()
{
    QString msg = player.GetIdentity().name + trUtf8(" is trying to connect...");
    qDebug() << msg.toLatin1().constData();
}
/*****************************************************************************/
void Client::slotSocketClosed()
{
    QString msg = player.GetIdentity().name + trUtf8(" connection has been closed.");
    qDebug() << msg.toLatin1().constData();
}
/*****************************************************************************/
void Client::slotSocketError(QAbstractSocket::SocketError code)
{
    QString message = player.GetIdentity().name;

    switch (code)
    {
    case QAbstractSocket::ConnectionRefusedError:
        message = trUtf8(": network error: connection refused.");
        break;
    case QAbstractSocket::HostNotFoundError:
        message = trUtf8(": network error: server not found.");
        break;
    default:
        message = trUtf8(": network error: data transmission failed.");
        break;
    }
    qDebug() << message.toLatin1().constData();
}
/*****************************************************************************/
void Client::slotSocketReadData()
{
    do
    {
        QDataStream in(&socket);
        if (DecodePacket(in) == true)
        {
            if (DoAction(in) == false)
            {
                // bad packet received, exit decoding
                return;
            }
        }
    }
    while (socket.bytesAvailable() > 0);
}
/*****************************************************************************/
bool Client::DoAction(QDataStream &in)
{
    quint8 command;
    bool ret = true;

    in >> command;
    switch (command)
    {
    case Protocol::SERVER_MESSAGE:
    {
        QString message;
        in >> message;
        emit sigMessage(message);
        break;
    }

    case Protocol::SERVER_REQUEST_IDENTITY:
    {
        quint8 p;

        in >> p;
        player.SetPlace((Place)p);
        SendIdentity();
        emit sigAssignedPlace((Place)p);
        break;
    }

    case Protocol::SERVER_PLAYERS_LIST:
    {
        quint8 nombre;
        QMap<Place, Identity> players;

        in >> nombre;
        for (int i = 0; i < nombre; i++)
        {
            Identity ident;
            quint8 place;

            in >> place;
            in >> ident;
            players[(Place)place] = ident;
        }
        emit sigPlayersList(players);
        break;
    }

    case Protocol::SERVER_SEND_CARDS:
    {
        int i;
        quint8 n;

        in >> n;
        info.Initialize();
        info.numberOfPlayers = n;
        if (n == 4)
        {
            player.GetDeck().clear();
            for (i = 0; i<info.GetNumberOfCards(); i++)
            {
                in >> n;
                player.GetDeck().append(TarotDeck::GetCard(n));
            }
            score.Reset();
            emit sigReceiveCards();
        }
        break;
    }

    case Protocol::SERVER_SELECT_PLAYER:
    {
        quint8 p;

        in >> p;
        info.Next();
        emit sigSelectPlayer((Place)p);
        break;
    }

    case Protocol::SERVER_REQUEST_BID:
    {
        quint8 c;

        in >> c; // Most important contract announced before
        emit sigRequestBid((Contract)c);
        break;
    }


    case Protocol::SERVER_SHOW_PLAYER_BID:
    {
        qint8 c, p;
        in >> p;
        in >> c;
        emit sigShowBid((Place)p, (Contract)c);
        break;
    }

    case Protocol::SERVER_SHOW_DOG:
    {
        dogDeck.clear();
        in >> dogDeck;
        dogDeck.Sort();
        emit sigShowDog();
        break;
    }

    case Protocol::SERVER_BUILD_DISCARD:
    {
        emit sigBuildDiscard();
        break;
    }

    case Protocol::SERVER_START_DEAL:
    {
        qint8 preneur;
        qint8 contrat;

        in >> preneur;
        in >> contrat;
        info.taker = (Place)preneur;
        info.contract = (Contract)contrat;
        currentTrick.clear();
        emit sigStartDeal((Place)preneur, (Contract)contrat);
        break;
    }

    case Protocol::SERVER_SHOW_HANDLE:
    {
        // FIXME: emit signal with handle deck
        break;
    }

    case Protocol::SERVER_PLAY_CARD:
    {
        emit sigPlayCard();
        break;
    }

    case Protocol::SERVER_SHOW_CARD:
    {
        quint8 id;
        quint8 tour;

        in >> id;
        in >> tour;
        currentTrick.append(TarotDeck::GetCard(id));
        emit sigShowCard((int)id, (Place)tour);
        break;
    }

    case Protocol::SERVER_DEAL_AGAIN:
    {
        emit sigDealAgain();
        break;
    }

    case Protocol::SERVER_END_OF_TRICK:
    {
        quint8 winner;
        in >> winner;
        emit sigWaitTrick((Place)winner);
        break;
    }

    case Protocol::SERVER_END_OF_DEAL:
    {
        in >> score;
        emit sigEndOfDeal();
        break;
    }

    case Protocol::SERVER_END_OF_GAME:
    {
        // FIXME
        emit sigEndOfGame();
        break;
    }

    default:
        QString msg = player.GetIdentity().name + trUtf8(": Unkown packet received.");
        qDebug() << msg.toLatin1().constData();
        ret = false;
        break;
    }
    return ret;
}
/*****************************************************************************/
void Client::SendIdentity()
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << player.GetIdentity();
    packet = BuildCommand(packet, Protocol::CLIENT_INFOS);
    socket.write(packet);
    socket.flush();
}
/*****************************************************************************/
void Client::SendChatMessage(const QString &message)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    QString msg;

    // We add the nickname before the message
    msg = player.GetIdentity().name + "> " + message;
    out << msg;
    packet = BuildCommand(packet, Protocol::CLIENT_MESSAGE);
    socket.write(packet);
    socket.flush();
}
/*****************************************************************************/
void Client::SendReady()
{
    QByteArray packet;
    packet = BuildCommand(packet, Protocol::CLIENT_READY);
    socket.write(packet);
    socket.flush();
}
/*****************************************************************************/
void Client::SendError()
{
    QByteArray packet;
    packet = BuildCommand(packet, Protocol::CLIENT_ERROR);
    socket.write(packet);
    socket.flush();
}
/*****************************************************************************/
void Client::SendBid(Contract c)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (quint8)c;
    packet = BuildCommand(packet, Protocol::CLIENT_BID);
    socket.write(packet);
    socket.flush();
}
/*****************************************************************************/
void Client::SendDog()
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << dogDeck;
    packet = BuildCommand(packet, Protocol::CLIENT_DISCARD);
    socket.write(packet);
    socket.flush();
}
/*****************************************************************************/
void Client::SendHandle()
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << handleDeck.size();
    out << handleDeck;
    packet = BuildCommand(packet, Protocol::CLIENT_HANDLE);
    socket.write(packet);
    socket.flush();
}
/*****************************************************************************/
void Client::SendCard(Card *c)
{
    if (c != NULL)
    {
        QByteArray packet;
        QDataStream out(&packet, QIODevice::WriteOnly);
        out << (quint8)c->GetId();
        packet = BuildCommand(packet, Protocol::CLIENT_CARD);
        socket.write(packet);
        socket.flush();
    }
    else
    {
        SendError();
    }
}
/*****************************************************************************/
void Client::SendSyncDog()
{
    info.sequence = Game::IDLE;

    QByteArray packet;
    packet = BuildCommand(packet, Protocol::CLIENT_SYNC_DOG);
    socket.write(packet);
    socket.flush();
}
/*****************************************************************************/
void Client::SendSyncTrick()
{
    info.sequence = Game::IDLE;
    currentTrick.clear();

    QByteArray packet;
    packet = BuildCommand(packet, Protocol::CLIENT_SYNC_TRICK);
    socket.write(packet);
    socket.flush();
}
/*****************************************************************************/
void Client::SendSyncHandle()
{
    QByteArray packet;
    packet = BuildCommand(packet, Protocol::CLIENT_SYNC_HANDLE);
    socket.write(packet);
    socket.flush();
}

//=============================================================================
// End of file Client.cpp
//=============================================================================
