/*=============================================================================
 * TarotClub - TarotServer.cpp
 *=============================================================================
 * Little web server to manage Tarot tables
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

#include "TarotServer.h"
#include "../Jeu.h"
#include <iostream>
using namespace std;

/*****************************************************************************/
TarotServer::TarotServer()
{
    listen(QHostAddress::Any, 8080);
    if (!isListening())
    {
        cerr << "Failed to bind to port 8080";
        qApp->quit();
    }

    Jeu::init();
}
/*****************************************************************************/
void TarotServer::start()
{
    config.load();
    lobby.setupTables(*config.getOptions());
    lobby.startGames();
}
/*****************************************************************************/
void TarotServer::incomingConnection(int socket)
{
    QTcpSocket *s = new QTcpSocket(this);
    connect(s, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
    s->setSocketDescriptor(socket);
}
/*****************************************************************************/
void TarotServer::readClient()
{
    // This slot is called when the client sent data to the server. The
    // server looks if it was a get request and sends a very simple HTML
    // document back.
    QTcpSocket *socket = (QTcpSocket *)sender();
    if (socket->canReadLine())
    {
        QStringList tokens = QString(socket->readLine()).split(QRegExp("[ \r\n][ \r\n]*"));
        if (tokens[0] == "GET")
        {
            QTextStream os(socket);
            os.setAutoDetectUnicode(true);
            os << "HTTP/1.0 200 Ok\r\n"
               "Content-Type: text/html; charset=\"utf-8\"\r\n"
               "\r\n"
               "<h1>Nothing to see here</h1>\n"
               << QDateTime::currentDateTime().toString() << "\n";
            socket->close();
            if (socket->state() == QTcpSocket::UnconnectedState)
            {
                delete socket;
            }
        }
    }
}
/*****************************************************************************/
void TarotServer::discardClient()
{
    QTcpSocket *socket = (QTcpSocket *)sender();
    socket->deleteLater();
}

//=============================================================================
// End of file TarotServer.cpp
//=============================================================================
