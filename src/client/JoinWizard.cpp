/*=============================================================================
 * TarotClub - JoinWizard.cpp
 *=============================================================================
 * Wizard pages to join a network server
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

#include <QTextStream>
#include "JoinWizard.h"
#include "ServerConfig.h"

/*****************************************************************************/
JoinWizardPage1::JoinWizardPage1(QWidget *parent) : QWizardPage(parent)
{
    ui.setupUi(this);
    setTitle(trUtf8("Connection to a TarotClub server"));
    setSubTitle(trUtf8("A game server can be created using a TarotClub cient or a dedicated server."));
}
/*****************************************************************************/
void JoinWizardPage1::initializePage()
{
    ui.ipAddress->setCursorPosition(0);
#ifndef QT_NO_DEBUG
    ui.ipAddress->setText("127.0.0.1");
#endif
    ui.portNumber->setValue(4242);
}
/*****************************************************************************/



/*****************************************************************************/
JoinWizardPage2::JoinWizardPage2(QWidget *parent) : QWizardPage(parent)
{
    ui.setupUi(this);
    setTitle(trUtf8("Connection to a TarotClub server"));
    setSubTitle(trUtf8("Choose your room and game table."));
}
/*****************************************************************************/
void JoinWizardPage2::initializePage()
{

}
/*****************************************************************************/



/*****************************************************************************/
JoinWizard::JoinWizard(QWidget *parent) : QWizard(parent)
{
    page1 = new JoinWizardPage1(this);
    page2 = new JoinWizardPage2(this);

    setPage(Page_Server, page1);
    setPage(Page_Lobby, page2);

    setPixmap(QWizard::WatermarkPixmap, QPixmap(":images/wizard.png"));
    setWizardStyle(QWizard::ModernStyle);

    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(slotServerConnection(int)));

    // événements sur le socket
    connect(&socket, SIGNAL(readyRead()), this, SLOT(socketReadData()));
    connect(&socket, SIGNAL(disconnected()), this, SLOT(socketClosed()));
    connect(&socket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(&socket, SIGNAL(hostFound()), this, SLOT(socketHostFound()));
}
/*****************************************************************************/
void JoinWizard::slotServerConnection(int id)
{
    if (id == Page_Lobby)
    {
        socket.close();
        socket.connectToHost(page1->getIp(), page1->getPort());
    }
}
/*****************************************************************************/
void JoinWizard::socketReadData()
{
    QTcpSocket *s = (QTcpSocket *)sender();
    if (s->canReadLine())
    {
        QStringList tokens = QString(s->readLine()).split(':', QString::SkipEmptyParts, Qt::CaseSensitive);
        if (tokens[0] == "SALOONS")
        {
            QStringList list;
            list.append(tokens[1]);
            page2->setSaloons(list);
        }
    }
}
/*****************************************************************************/
void JoinWizard::socketConnected()
{
    QString txt = trUtf8("Connected to the server.");
    page2->setInfos(txt);

    // get list of playing rooms
    QTextStream os(&socket);
    os << "GET:SALOONS:\r\n";
    os.flush();
}
/*****************************************************************************/
void JoinWizard::socketHostFound()
{
    QString txt = trUtf8("Connection to the server ....");
    page2->setInfos(txt);
}
/*****************************************************************************/
void JoinWizard::socketClosed()
{
    QString txt = trUtf8("The connection has been closed by the server.");
    page2->setInfos(txt);
}
/*****************************************************************************/
void JoinWizard::socketError(QAbstractSocket::SocketError code)
{
    QString txt = trUtf8("Socket error - code: ") + QString().setNum((int)code);
    page2->setInfos(txt);
}

//=============================================================================
// End of file JoinWizard.cpp
//=============================================================================
