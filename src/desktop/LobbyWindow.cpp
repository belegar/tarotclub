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
#include "LobbyWindow.h"
#include "ServerConfig.h"

LobbyWindow::LobbyWindow(QWidget *parent = 0)
    : QDialog(parent)
{
    ui.setupUi(this);
    ui.ipAddress->setCursorPosition(0);
    ui.portNumber->setValue(4242);

#ifdef QT_DEBUG
    ui.ipAddress->setText("127.0.0.1");
#endif

    connect(ui.roomList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotRoomSelected(QListWidgetItem *)));
    connect(ui.tableList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotTableSelected(QListWidgetItem *)));

    // socket events
    connect(&socket, SIGNAL(readyRead()), this, SLOT(socketReadData()));
    connect(&socket, SIGNAL(disconnected()), this, SLOT(socketClosed()));
    connect(&socket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(&socket, SIGNAL(hostFound()), this, SLOT(socketHostFound()));

    connect(ui.connectButton, &QPushButton::clicked, this, &LobbyWindow::slotConnect);
    connect(ui.joinButton, &QPushButton::clicked, this, &LobbyWindow::slotJoin);
    connect(ui.closeButton, &QPushButton::clicked, this, &LobbyWindow::slotClose);

    selectedTable.isValid = false;
}

/*****************************************************************************/
void LobbyWindow::slotRoomSelected(QListWidgetItem *item)
{
    selectedTable.isValid = false;
    // get list of tables in this room
    QTextStream os(&socket);
    os << "GET:TABLES:" << item->text();
    os.flush();
}
/*****************************************************************************/
void LobbyWindow::slotTableSelected(QListWidgetItem *item)
{
    selectedTable.isValid = false;
    // get the tcp/ip port of this table
    QTextStream os(&socket);
    os << "GET:PORT:" << ui.roomList->selectedItems().at(0)->text() << "," << item->text();
    os.flush();
}
/*****************************************************************************/
void LobbyWindow::slotConnect()
{
    socket.close();
    socket.connectToHost(ui.ipAddress->text(), ui.portNumber->value());
    selectedTable.isValid = false;
}
/*****************************************************************************/
LobbyWindow::Connection LobbyWindow::GetTableConnection()
{
    return selectedTable;
}
/*****************************************************************************/
void LobbyWindow::slotJoin()
{
    socket.close();
    this->accept();
}
/*****************************************************************************/
void LobbyWindow::slotClose()
{
    socket.close();
    this->reject();
}
/*****************************************************************************/
void LobbyWindow::socketReadData()
{
    QTcpSocket *s = (QTcpSocket *)sender();
    if (s->canReadLine())
    {
        QString line = s->readLine();
        // remove new line character
        line.remove('\n');
        QStringList tokens = line.split(':', QString::SkipEmptyParts, Qt::CaseSensitive);
        if (tokens[0] == "SALOON")
        {
            QStringList list = tokens[1].split(',');
            ui.roomList->clear();
            ui.roomList->addItems(list);
        }
        else if (tokens[0] == "TABLES")
        {
            QStringList list = tokens[1].split(',');
            ui.tableList->clear();
            ui.tableList->addItems(list);
        }
        else if (tokens[0] == "PORT")
        {
            selectedTable.port = tokens[1].toUInt();
            selectedTable.ip = ui.ipAddress->text();
            selectedTable.isValid = true;
        }
    }
}
/*****************************************************************************/
void LobbyWindow::socketConnected()
{
    QString txt = trUtf8("Connected to the server.");
    ui.infoLabel->setText(txt);

    // get list of playing rooms
    QTextStream os(&socket);
    os << "GET:INFOS\n";
    os.flush();
}
/*****************************************************************************/
void LobbyWindow::socketHostFound()
{
    QString txt = trUtf8("Connection to the server ....");
    ui.infoLabel->setText(txt);
}
/*****************************************************************************/
void LobbyWindow::socketClosed()
{
    QString txt = trUtf8("The connection has been closed by the server.");
    ui.infoLabel->setText(txt);
}
/*****************************************************************************/
void LobbyWindow::socketError(QAbstractSocket::SocketError code)
{
    QString txt = trUtf8("Socket error - code: ") + QString().setNum((int)code);
    ui.infoLabel->setText(txt);
}

//=============================================================================
// End of file JoinWizard.cpp
//=============================================================================
