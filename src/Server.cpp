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
{

}
/*****************************************************************************/
void Server::SetMaximumPlayers(int n)
{
    table.maximumPlayers = n;
}
/*****************************************************************************/
void Server::slotNewConnection()
{
    QTcpSocket *cnx = server.nextPendingConnection();
    int n = players.size();

    if (n == table.maximumPlayers)
    {
        SendErrorServerFull(cnx);
        cnx->close();
    }
    else
    {
        Player *player = new Player();
        Place p;

        p = table.reserveFreePlace(); // TODO: add protection here, test HYPERSPACE retun?
        player->setPlace(p);

        connect(cnx, SIGNAL(disconnected()), this, SLOT(slotClientClosed()));
        connect(cnx, SIGNAL(readyRead()), this, SLOT(slotReadData()));

        players[cnx] = player; // on ajoute ce client à la liste
        askIdentity(cnx, p);
    }
}
/*****************************************************************************/
void TarotEngine::setOptions(ServerOptions &opt)
{
    int i;

    options = opt;
    for (i = 0; i < 3; i++)
    {
        bots[i].setIdentity(options.bots[i]);
        bots[i].setTimeBeforeSend(options.timer);
    }
}
/*****************************************************************************/
QList<Identity> TarotEngine::getConnectedPlayers()
{
    QList<Identity> idents;

    QMapIterator<QTcpSocket *, Player *> i(players);
    while (i.hasNext())
    {
        i.next();
        idents.append(*i.value()->getIdentity());
    }
    return idents;
}
/*****************************************************************************/
int TarotEngine::getNumberOfConnectedPlayers()
{
    return players.size();
}
/*****************************************************************************/
void TarotEngine::newServerGame()
{
    int i;
    int port;

    closeClients();
    server.close();
    table.freePlace(BROADCAST);

    // 4 joueurs max + une connexion en plus pour avertir aux nouveaux arrivants
    // que le serveur est plein
    server.setMaxPendingConnections(NB_PLAYERS + 1);

    if (gameType == NET_GAME_SERVER)
    {
        port = options.port;
    }
    else
    {
        // if local game, always use the default port
        port = DEFAULT_PORT;
    }
    server.listen(QHostAddress::LocalHost, port);

    // Init everything
    gameState.Initialize();
    score.Initialize();
    dealCounter = 0;
    emit sigPrintMessage("Server started.\r\n");
}
/*****************************************************************************/
void TarotEngine::connectBots()
{
    int i;

    qApp->processEvents(QEventLoop::AllEvents, 100);
    for (i = 0; i < 3; i++)
    {
        bots[i].connectToHost("127.0.0.1", options.port);
        qApp->processEvents(QEventLoop::AllEvents, 100);
    }
}
/*****************************************************************************/
void CloseClients()
{
    QMapIterator<QTcpSocket *, Player *> i(players);
    while (i.hasNext())
    {
        i.next();
        QTcpSocket *cnx = i.key();
        connect(cnx, SIGNAL(disconnected()), cnx, SLOT(deleteLater()));
        cnx->close();
        players.remove(cnx);
    }
}
/*****************************************************************************/
void Server::slotClientClosed()
{
    // retrieve our sender QTcpSocket
    QTcpSocket *cnx = qobject_cast<QTcpSocket *>(sender());
    Player *player = players[cnx];
    if (player == NULL)
    {
        return;
    }
    QString message = "Le joueur " + player->getName() + " a quitté la partie.";
    connect(cnx, SIGNAL(disconnected()), cnx, SLOT(deleteLater()));
    cnx->close();
    players.remove(cnx);
    sendMessage(message, BROADCAST);
    sendPlayersList();

    // FIXME: if a player has quit during a game, replace it by a bot
}
/*****************************************************************************/
void Server::slotReadData()
{
    // retrieve our sender QTcpSocket
    QTcpSocket *cnx = qobject_cast<QTcpSocket *>(sender());
    qint64 bytes = cnx->bytesAvailable();

    QDataStream in(cnx);

    DecodePacket(in, bytes);
}
/*****************************************************************************/
void Server::DoAction(QDataStream &in)
{
    // First byte is the command
    quint8 cmd;
    in >> cmd;

    switch (cmd)
    {
        case Command::CLIENT_MESSAGE:
        {
            // TODO: REFUSER SI MAUVAISE SEQUENCE EN COURS
            QString message;
            in >> message;
            sendMessage(message, BROADCAST);
            emit sigPrintMessage(QString("Client message: ") + message);
            break;
        }

        case Command::CLIENT_INFOS:
        {
            Identity ident;
            QString version;

            in >> version; // TODO: test protocol version or already done by Qt in lower layers?
            in >> ident;

            ident.avatar = ":/images/avatars/" + ident.avatar;
            players[cnx]->setIdentity(ident);
            QString m = "Le joueur " + ident.name + " a rejoint la partie.";
            sendMessage(m , BROADCAST);
            emit sigPrintMessage(m);

            if (getNumberOfConnectedPlayers() == NB_PLAYERS)
            {
                sendPlayersList();
                gameState.state = GAME_STARTED;
                nouvelleDonne();
            }

            /*
              TODO: same nickname allowed or don't care ???
                     bool ok = false;
                     // On cherche si le nick n'existe pas déjà
                     QMapIterator<QTcpSocket*, Player*> i(players);
                     while (i.hasNext()) {
                        i.next();
                        if (i.value()->getName() == ident.name) {
                           ok = true;
                           break;
                        }
                     }

                     if( ok == true ) {
                        // On prévient le joueur que son nick est déjà pris
                        QString message = "Votre pseudonyme \"" + ident.name + "\" existe déjà sur le serveur."+
                           " Changez le et reconnectez-vous.";

                        // et on annule sa connexion
                        sendMessage( message, players[cnx]->getPlace() );
                        connect(cnx, SIGNAL(disconnected()), cnx, SLOT(deleteLater()));
                        cnx->close();
                        players.remove(cnx);

                     } else {
                     }
            */
            break;
        }

        /**
         * Contrat d'un joueur
         */
        case NET_CLIENT_ENCHERE:
        {
            quint8 c;
            // TODO: REFUSER SI MAUVAISE SEQUENCE EN COURS
            in >> c;
            if ((Contrat)c > infos.contrat)
            {
                infos.contrat = (Contrat)c;
                infos.preneur = tour;
            }
            sendBid(tour, (Contrat)c);
            sequenceEncheres();
            break;
        }

        case NET_CLIENT_VU_CHIEN:
            if (sequence == WAIT_CHIEN)
            {
                cptVuChien++;
                if (cptVuChien >= NB_PLAYERS)
                {
                    sequence = CHIEN;
                    sendDoChien(); // On ordonne au preneur de faire le chien et on attend
                }
            }
            break;

        case NET_CLIENT_VU_PLI:
            if (sequence == SEQ_WAIT_PLI)
            {
                cptVuPli++;
                if (cptVuPli >= NB_PLAYERS)
                {
                    NextPlayer();
                }
            }
            break;

            /**
             * Chien d'un client
             */
        case NET_CLIENT_CHIEN:
        {
            quint8 id;
            int nb_cartes_chien;
            int i;
            Card *c;
            // TODO: REFUSER SI MAUVAISE SEQUENCE EN COURS
            if (NB_PLAYERS == 3)
            {
                nb_cartes_chien = 6;
            }
            else if (NB_PLAYERS == 4)
            {
                nb_cartes_chien = 6;
            }
            else     // 5 joueurs
            {
                nb_cartes_chien = 3;
            }
            deckChien.clear();
            for (i = 0; i < nb_cartes_chien; i++)
            {
                in >> id;
                c = Jeu::getCard(id);
                deckChien.append(c);
            }
            sequence = GAME;
            tour = donneur;
            sendDepartDonne();
            jeu();
            break;
        }

        /**
         * Un client a déclaré une poignée
         */
        case NET_CLIENT_POIGNEE:
        {
            quint8 dummy;
            in >> dummy;
            quint8 id;
            Poignee p;
            // TODO: REFUSER SI MAUVAISE SEQUENCE EN COURS
            // TODO: add protection, limits ...
            poigneeDeck.clear();
            for (int i = 0; i < dummy; i++)
            {
                in >> id;
                poigneeDeck.append(Jeu::getCard(id));
            }
            // TODO: add robustness here!!
            if (dummy == 10)
            {
                p = P_SIMPLE;
            }
            else if (dummy == 13)
            {
                p = P_DOUBLE;
            }
            else
            {
                p = P_TRIPLE;
            }

            if (players[cnx]->getPlace() == infos.preneur)
            {
                score.setPoigneeAttaque(p);
            }
            else
            {
                score.setPoigneeDefense(p);
            }
            /*
             Tester :
             1) L'origine du client (sud, est ... ip ??)
             2) La validité de la poignée (présence dans le deck du joueur, utilisation de l'excuse)
             3) La poignée doit être déclarée avant de jouer la première carte
             4) Puis envoyer la poignée à tout le monde ...
            */
            break;
        }

        /**
         * Carte d'un client
         */
        case NET_CLIENT_CARTE:
        {
            quint8 id;
            Card *c;
            // TODO: tester la validité de la carte (ID + présence dans le jeu du joueur)
            // si erreur : logguer et prévenir quelqu'un ??
            // TODO: REFUSER SI MAUVAISE SEQUENCE EN COURS
            in >> id;
            c = Jeu::getCard(id);
            mainDeck.append(c);
            players[cnx]->removeCard(c);
            sendCard(c);
            jeu();
            break;
        }

        case NET_CLIENT_READY:
            if (sequence == SEQ_WAIT_PLAYER)
            {
                cptVuDonne++;
                if (cptVuDonne >= NB_PLAYERS)
                {
                    nouvelleDonne();
                }
            }
            break;

        default:
            break;
    }
}
/*****************************************************************************/
void Server::SendErrorServerFull(QTcpSocket *cnx)
{
    QString message = "Le serveur est complet.";
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QT_STREAMVER);
    out << (quint16)0 << (quint8)NET_MESSAGE
        << message
        << (quint16)0xFFFF;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    connect(cnx, SIGNAL(disconnected()), cnx, SLOT(deleteLater()));
    cnx->write(block);
    cnx->flush();
}
/*****************************************************************************/
void Server::SendAskIdentity(QTcpSocket *cnx, Place p)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QT_STREAMVER);
    out << (quint16)0 << (quint8)NET_IDENTIFICATION
        << (quint8)p // assignate place around table
        << (quint16)0xFFFF;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    cnx->write(block);
    cnx->flush();
}
/*****************************************************************************/
void Server::SendCards(Place p, quint8 *params)
{
    int j;
    QByteArray block;

    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QT_STREAMVER);
    out << (quint16)0 << (quint8)NET_RECEPTION_CARTES
        << (quint8)4;
    for (j = 0; j < NB_HAND_CARDS; j++)
    {
        out << (quint8)params[j];
    }
    out << (quint16)0xFFFF;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    QTcpSocket *s = getConnection(p);
    s->write(block);
    s->flush();
}
/*****************************************************************************/
void Server::SendAskBid(Contrat c)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QT_STREAMVER);
    out << (quint16)0 << (quint8)NET_DEMANDE_ENCHERE
        << (quint8)c   // le contrat précédent
        << (quint16)0xFFFF;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    QTcpSocket *s = getConnection(tour);
    s->write(block);
    s->flush();
}
/*****************************************************************************/
void Server::SendBid(Place p, Contrat c)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QT_STREAMVER);
    out << (quint16)0 << (quint8)NET_ENCHERE_JOUEUR
        << (quint8)p   // la place de celui qui vient d'enchérir
        << (quint8)c   // le contrat à afficher
        << (quint16)0xFFFF;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    broadcast(block);
}
/*****************************************************************************/
void Server::SendSelectPlayer(Place p)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QT_STREAMVER);
    out << (quint16)0 << (quint8)NET_SELECTION_JOUEUR
        << (quint8)p   // la place du joueur qui est en train de jouer
        << (quint16)0xFFFF;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    broadcast(block);
}
/*****************************************************************************/
void Server::SendMessage(const QString &message, Place p)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QT_STREAMVER);
    out << (quint16)0 << (quint8)NET_MESSAGE
        << message
        << (quint16)0xFFFF;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    if (p == BROADCAST)
    {
        broadcast(block);
    }
    else
    {
        QTcpSocket *s = getConnection(p);
        s->write(block);
        s->flush();
    }
}
/*****************************************************************************/
void Server::SendPlayersList()
{
    QByteArray block;
    QList<Identity> idents;

    idents = getConnectedPlayers();
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QT_STREAMVER);
    out << (quint16)0 << (quint8)NET_LISTE_JOUEURS
        << (quint8)idents.size(); // nombre de joueurs

    for (int i = 0; i < idents.size(); i++)
    {
        out << idents[i];
    }
    out << (quint16)0xFFFF;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    broadcast(block);   // to connected clients
}
/*****************************************************************************/
void Server::SendShowChien()
{
    int i;
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out.setVersion(QT_STREAMVER);
    out << (quint16)0 << (quint8)NET_MONTRE_CHIEN;
    for (i = 0; i < deckChien.count(); i++)
    {
        out << (quint8)deckChien.at(i)->getId();
    }
    out << (quint16)0xFFFF;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    broadcast(block);   // to connected clients
}
/*****************************************************************************/
void Server::SendDoChien()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out.setVersion(QT_STREAMVER);
    out << (quint16)0 << (quint8)NET_FAIT_CHIEN
        << (quint16)0xFFFF;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    QTcpSocket *s = getConnection(infos.preneur);
    s->write(block);
    s->flush();
}
/*****************************************************************************/
void Server::SendDepartDonne()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out.setVersion(QT_STREAMVER);
    out << (quint16)0 << (quint8)NET_DEPART_DONNE
        << (quint8)infos.preneur
        << (quint8)infos.contrat
        << (quint16)0xFFFF;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    broadcast(block);   // to connected clients
}
/*****************************************************************************/
void Server::SendJoueCarte()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out.setVersion(QT_STREAMVER);
    out << (quint16)0 << (quint8)NET_JOUE_CARTE
        << (quint16)0xFFFF;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    QTcpSocket *s = getConnection(tour);
    s->write(block);
    s->flush();
}
/*****************************************************************************/
void Server::SendCard(Card *c)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out.setVersion(QT_STREAMVER);
    out << (quint16)0 << (quint8)NET_MONTRE_CARTE
        << (quint8)c->getId()
        << (quint8)tour
        << (quint16)0xFFFF;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    broadcast(block);   // to connected clients
}
/*****************************************************************************/
void Server::SendRedist()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out.setVersion(QT_STREAMVER);
    out << (quint16)0 << (quint8)NET_SERVER_REDIST
        << (quint16)0xFFFF;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    broadcast(block);   // to connected clients
}
/*****************************************************************************/
void Server::SendFinDonne(ScoreInfo &score_inf, bool lastDeal, float pointsTour)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out.setVersion(QT_STREAMVER);
    out << (quint16)0 << (quint8)NET_FIN_DONNE
        << score_inf;

    if (lastDeal == true)
    {
        out << (quint8)1;
    }
    else
    {
        out << (quint8)0;
    }

    out << (quint8)tour // winner of the current turn
        << (quint32)pointsTour // points won by the taker
        << (quint16)0xFFFF;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    broadcast(block);   // to connected clients
}
/*****************************************************************************/
void Server::SendWaitPli(float pointsTour)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out.setVersion(QT_STREAMVER);
    out << (quint16)0 << (quint8)NET_SERVER_WAIT_PLI
        << (quint8)tour // winner of the current turn
        << (quint32)pointsTour // points won by the taker
        << (quint16)0xFFFF;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    broadcast(block);   // to connected clients
}
/*****************************************************************************/
void Server::broadcast(QByteArray &block)
{
    QMapIterator<QTcpSocket *, Player *> i(players);
    while (i.hasNext())
    {
        i.next();
        i.key()->write(block);
        i.key()->flush();
    }
}


//=============================================================================
// End of file Server.cpp
//=============================================================================
