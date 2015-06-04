/*=============================================================================
 * TarotClub - LobbyDock.cpp
 *=============================================================================
 * Lobby window, includes a chat area and the tables available on the server
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
#include <QHostInfo>
#include <QtNetwork>
#include "LobbyDock.h"
#include "ServerConfig.h"
#include "Lobby.h"

/*****************************************************************************/
LobbyDock::LobbyDock(QWidget *parent = 0)
    : QDockWidget(tr("Online game lobby"), parent)
    , mConnected(false)
{
    ui.setupUi(this);

    connect(ui.connectButton, &QPushButton::clicked, this, &LobbyDock::slotConnect);
    connect(ui.disconnectButton, &QPushButton::clicked, this, &LobbyDock::slotDisconnect);
    connect(ui.joinButton, &QPushButton::clicked, this, &LobbyDock::slotJoin);
    connect(ui.quitButton, &QPushButton::clicked, this, &LobbyDock::slotQuit);
    connect(ui.chatText, &QLineEdit::returnPressed, this, &LobbyDock::slotReturnPressed);

    Initialize();
}
/*****************************************************************************/
void LobbyDock::slotConnect()
{
    ui.infoLabel->setText(trUtf8("Connecting ..."));
    std::uint32_t item = ui.serverList->currentIndex();
    if (item < mServerList.size())
    {
        QHostInfo info = QHostInfo::fromName(mServerList[item].address.c_str());
        QString gamePort = QString().setNum(mServerList[item].game_tcp_port);

        if (info.error() == QHostInfo::NoError)
        {
            QList<QHostAddress>	addresses = info.addresses();
            // Try each IP address to find any suitable server
            for (QList<QHostAddress>::iterator iter = addresses.begin(); iter != addresses.end(); ++iter)
            {
                emit sigConnect(iter->toString(), (std::uint16_t)gamePort.toUShort());
            }
        }
    }
}
/*****************************************************************************/
void LobbyDock::slotDisconnect()
{
    emit sigDisconnect();
}
/*****************************************************************************/
void LobbyDock::slotChatMessage(QString message)
{
    if (mConnected)
    {
        ui.textArea->append(message);
    }
}
/*****************************************************************************/
void LobbyDock::SystemMessage(const QString &message)
{
    ui.textArea->append(QString("<b>") + message + QString("</b>"));
}
/*****************************************************************************/
void LobbyDock::SetPlayersNames(const QMap<std::uint32_t, Identity> &players)
{
    if (mConnected)
    {
        ui.infoLabel->setText(trUtf8("Connected."));
        ui.playerList->clear();
        mPlayerList = players;
        for (QMap<std::uint32_t, Identity>::const_iterator iter = mPlayerList.begin(); iter != mPlayerList.end(); ++iter)
        {
            ui.playerList->addItem(iter.value().nickname.c_str());
        }
    }
}
/*****************************************************************************/
void LobbyDock::SetTableStatus(std::uint32_t tableId, bool status)
{
    (void) tableId; // FIXME: add information about the connection using this id
    if (status)
    {
        ui.joinButton->setEnabled(false);
        ui.quitButton->setEnabled(true);
    }
    else
    {
        ui.joinButton->setEnabled(true);
        ui.quitButton->setEnabled(false);
    }
}
/*****************************************************************************/
void LobbyDock::DisconnectedFromServer()
{
    Initialize();
}
/*****************************************************************************/
void LobbyDock::SetTables(const QMap<QString, std::uint32_t> &tableList)
{
    mConnected = true;
    ui.tableList->clear();
    mTableList = tableList;
    for (QMap<QString, std::uint32_t>::const_iterator iter = tableList.begin(); iter != tableList.end(); ++iter)
    {
        ui.tableList->addItem(iter.key());
    }

    ui.connectButton->setEnabled(false);
    ui.disconnectButton->setEnabled(true);
    ui.joinButton->setEnabled(true);
}
/*****************************************************************************/
void LobbyDock::SetServersList(const std::vector<ServerInfo> &servers)
{
    mServerList = servers;

#if TAROT_DEBUG
    ServerInfo srv;

    srv.game_tcp_port = 4269;
    srv.address = "192.168.1.30";
    mServerList.push_back(srv);

    srv.address = "192.168.1.12";
    mServerList.push_back(srv);

    srv.address = "127.0.0.1";
    mServerList.push_back(srv);
#endif

    UpdateServersList();
}
/*****************************************************************************/
void LobbyDock::UpdateServersList()
{
    ui.serverList->clear();
    for (std::vector<ServerInfo>::iterator iter = mServerList.begin(); iter != mServerList.end(); ++iter)
    {
        QString server = QString(iter->address.c_str());
        ui.serverList->addItem(server);
    }
}
/*****************************************************************************/
void LobbyDock::slotJoin()
{
    // Gets the table name
    QListWidgetItem * item = ui.tableList->currentItem();
    if (item != NULL)
    {
        QString name = item->text();
        if (mTableList.contains(name))
        {
            emit sigJoinTable(mTableList[name]);
        }
    }
}
/*****************************************************************************/
void LobbyDock::slotQuit()
{
    // Gets the table name
    QListWidgetItem * item = ui.tableList->currentItem();
    if (item != NULL)
    {
        QString name = item->text();
        if (mTableList.contains(name))
        {
            emit sigQuitTable(mTableList[name]);
        }
    }
}
/*****************************************************************************/
void LobbyDock::Initialize()
{  
    mConnected = false;
    ui.joinButton->setEnabled(false);
    ui.quitButton->setEnabled(false);
    ui.connectButton->setEnabled(true);
    ui.disconnectButton->setEnabled(false);
    ui.infoLabel->setText(trUtf8("Not connected."));
    mTableList.clear();
    mPlayerList.clear();
    ui.tableList->clear();
    ui.playerList->clear();
}
/*****************************************************************************/
void LobbyDock::slotReturnPressed()
{
    QString message = ui.chatText->text();
    if (message.length() == 0)
    {
        return;
    }
    ui.chatText->clear();
    ui.chatText->setFocus();
    if (mConnected)
    {
        emit sigEmitMessage(message);
    }
}

//=============================================================================
// End of file LobbyDock.cpp
//=============================================================================
