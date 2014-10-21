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

    connect(ui.tableList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotTableSelected(QListWidgetItem *)));

    connect(ui.connectButton, &QPushButton::clicked, this, &LobbyWindow::slotConnect);
    connect(ui.joinButton, &QPushButton::clicked, this, &LobbyWindow::slotJoin);
    connect(ui.closeButton, &QPushButton::clicked, this, &LobbyWindow::slotClose);

    Initialize();
}
/*****************************************************************************/
void LobbyWindow::slotRoomSelected(QListWidgetItem *item)
{
    selectedTable.isValid = false;

}
/*****************************************************************************/
void LobbyWindow::slotTableSelected(QListWidgetItem *item)
{
    selectedTable.isValid = false;

}
/*****************************************************************************/
void LobbyWindow::slotConnect()
{
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
    this->accept();
}
/*****************************************************************************/
void LobbyWindow::slotClose()
{
    this->reject();
}
/*****************************************************************************/
void LobbyWindow::Initialize()
{
    selectedTable.isValid = false;
    QString txt = trUtf8("Not connected.");
    ui.infoLabel->setText(txt);
    ui.tableList->clear();
}
/*****************************************************************************/
void LobbyWindow::socketReadData()
{
}
/*****************************************************************************/
void LobbyWindow::socketConnected()
{
    QString txt = trUtf8("Connected to the server.");
    ui.infoLabel->setText(txt);

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
