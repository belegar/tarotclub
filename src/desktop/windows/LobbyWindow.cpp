/*=============================================================================
 * TarotClub - LobbyWindow.cpp
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
#include "LobbyWindow.h"
#include "ServerConfig.h"
#include "Lobby.h"
#include "ui_NewServerUI.h"

/*****************************************************************************/
LobbyWindow::LobbyWindow(QWidget *parent = 0)
    : QDialog(parent, Qt::Window)
{
    setModal(false);
    ui.setupUi(this);

    connect(ui.connectButton, &QPushButton::clicked, this, &LobbyWindow::slotConnect);
    connect(ui.disconnectButton, &QPushButton::clicked, this, &LobbyWindow::slotDisconnect);
    connect(ui.joinButton, &QPushButton::clicked, this, &LobbyWindow::slotJoin);
    connect(ui.quitButton, &QPushButton::clicked, this, &LobbyWindow::slotQuit);
    connect(ui.closeButton, &QPushButton::clicked, this, &LobbyWindow::slotClose);
    connect(ui.checkServerButton, &QPushButton::clicked, this, &LobbyWindow::slotCheckServer);
    connect(ui.addServerButton, &QPushButton::clicked, this, &LobbyWindow::slotAddServer);
    connect(ui.removeServerButton, &QPushButton::clicked, this, &LobbyWindow::slotRemoveServer);
    connect(ui.addBotButton, &QPushButton::clicked, this, &LobbyWindow::slotAddBot);
    connect(ui.removeBotButton, &QPushButton::clicked, this, &LobbyWindow::slotRemoveBot);
    connect(ui.chatText, &QLineEdit::returnPressed, this, &LobbyWindow::slotReturnPressed);

    Initialize();
}
/*****************************************************************************/
void LobbyWindow::slotConnect()
{
    ui.infoLabel->setText(trUtf8("Connecting ..."));
    std::uint32_t item = ui.serverList->currentRow();
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
                if (CheckServer())
                {
                    // It seems that we have found a valid server, try to connect
                    emit sigConnect(iter->toString(), (std::uint16_t)gamePort.toUShort());
                    break;
                }
                else
                {
                    ui.textArea->append(tr("Invalid server"));
                }
            }
        }
    }
}
/*****************************************************************************/
void LobbyWindow::slotDisconnect()
{
    emit sigDisconnect();
}
/*****************************************************************************/
void LobbyWindow::slotMessage(std::string message)
{
    ui.textArea->append(QString(message.c_str()));
}
/*****************************************************************************/
void LobbyWindow::slotConnectionFailure()
{
     ui.infoLabel->setText(trUtf8("Connection to the seveur failed!"));
}
/*****************************************************************************/
void LobbyWindow::SetPlayersNames(const std::map<std::uint32_t, std::string> &players)
{
    ui.infoLabel->setText(trUtf8("Connected."));
    ui.playerList->clear();
    mPlayerList = players;
    for (std::map<std::uint32_t, std::string>::const_iterator iter = mPlayerList.begin(); iter != mPlayerList.end(); ++iter)
    {
        ui.playerList->addItem(QString(iter->second.c_str()));
    }
}
/*****************************************************************************/
void LobbyWindow::SetTableStatus(std::uint32_t tableId, bool status)
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
void LobbyWindow::DisconnectedFromServer()
{
    Initialize();
}
/*****************************************************************************/
void LobbyWindow::SetTables(const std::map<std::string, std::uint32_t> &tableList)
{
    ui.tableList->clear();
    mTableList = tableList;
    for (std::map<std::string, std::uint32_t>::const_iterator iter = tableList.begin(); iter != tableList.end(); ++iter)
    {
        ui.tableList->addItem(QString(iter->first.c_str()));
    }
}
/*****************************************************************************/
void LobbyWindow::SetServersList(const std::vector<ServerInfo> &servers)
{
    mServerList = servers;

#if TAROT_DEBUG
    ServerInfo srv;

    srv.game_tcp_port = 4269;
    srv.web_tcp_port = 8080;
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
void LobbyWindow::UpdateServersList()
{
    ui.serverList->clear();
    for (std::vector<ServerInfo>::iterator iter = mServerList.begin(); iter != mServerList.end(); ++iter)
    {
        QString server = QString(iter->address.c_str()) + QString(" (%1), (%2)").arg(iter->game_tcp_port).arg(iter->web_tcp_port);
        ui.serverList->addItem(server);
    }

    if (mServerList.size() > 0)
    {
        // Select first element
        ui.serverList->setCurrentRow(0);
    }
}
/*****************************************************************************/
std::vector<ServerInfo> LobbyWindow::GetServersList()
{
    return mServerList;
}
/*****************************************************************************/
void LobbyWindow::slotJoin()
{
    // Gets the table name
    QListWidgetItem * item = ui.tableList->currentItem();
    if (item != NULL)
    {
        std::string name = item->text().toStdString();
        if (mTableList.find(name) != mTableList.end())
        {
            emit sigJoinTable(mTableList[name]);
        }
    }
}
/*****************************************************************************/
void LobbyWindow::slotQuit()
{
    // Gets the table name
    QListWidgetItem * item = ui.tableList->currentItem();
    if (item != NULL)
    {
        std::string name = item->text().toStdString();
        if (mTableList.find(name) != mTableList.end())
        {
            emit sigQuitTable(mTableList[name]);
        }
    }
}
/*****************************************************************************/
void LobbyWindow::slotCheckServer()
{
    ui.serverStatus->setText(tr("Checking ..."));

    if (CheckServer())
    {
        ui.serverStatus->setText(tr("Server OK"));
    }
    else
    {
        ui.serverStatus->setText(tr("Server unavailable :("));
    }
}
/*****************************************************************************/
void LobbyWindow::slotAddServer()
{
    QDialog *dialog = new QDialog(this);
    Ui::newServerUI srvUi;

    srvUi.setupUi(dialog);

    if (dialog->exec() == QDialog::Accepted)
    {
        ServerInfo server;
        server.address = srvUi.address->text().toStdString();
        server.game_tcp_port = srvUi.gamePort->value();
        server.web_tcp_port = srvUi.webPort->value();

        mServerList.push_back(server);
        UpdateServersList();
        emit sigSaveServersConfiguration();
    }
}
/*****************************************************************************/
void LobbyWindow::slotRemoveServer()
{
    std::uint32_t item = ui.serverList->currentRow();
    if (item < mServerList.size())
    {
        mServerList.erase(mServerList.begin() + item);

        UpdateServersList();
        emit sigSaveServersConfiguration();
    }
}
/*****************************************************************************/
void LobbyWindow::slotAddBot()
{

}
/*****************************************************************************/
void LobbyWindow::slotRemoveBot()
{

}
/*****************************************************************************/
bool LobbyWindow::CheckServer()
{
    bool ret = false;
    std::uint32_t item = ui.serverList->currentRow();
    if (item < mServerList.size())
    {
        QHostInfo info = QHostInfo::fromName(mServerList[item].address.c_str());
        QString webPort = QString().setNum(mServerList[item].web_tcp_port);

        if (info.error() == QHostInfo::NoError)
        {
            QList<QHostAddress>	addresses = info.addresses();
            // Try each IP address to find any suitable server
            for (QList<QHostAddress>::iterator iter = addresses.begin(); iter != addresses.end(); ++iter)
            {
                QString reply;
                QString request = QString("http://") + iter->toString() + ":" + webPort;
                if (RequestHttp(request + "/version", reply))
                {
                    // ok, found it, check the version string
                    if (reply == QString(Lobby::LOBBY_VERSION_STRING.c_str()))
                    {
                        ret = true;
                    }
                }
            }
        }
    }

    return ret;
}
/*****************************************************************************/
void LobbyWindow::slotClose()
{
    this->hide();
}
/*****************************************************************************/
void LobbyWindow::Initialize()
{  
    ui.joinButton->setEnabled(true);
    ui.quitButton->setEnabled(false);
    ui.infoLabel->setText(trUtf8("Not connected."));
    mTableList.clear();
    ui.tableList->clear();
    ui.playerList->clear();
    ui.serverStatus->setText("-");
}
/*****************************************************************************/
bool LobbyWindow::RequestHttp(const QString &request, QString &reply)
{
    bool ret = false;
    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the eventl-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    QUrl url(request);
    // the HTTP request
    QNetworkRequest req(url);

    QNetworkReply *netReply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (netReply->error() == QNetworkReply::NoError)
    {
        // Everything is ok => reply->readAll()
        reply = netReply->readAll();
        ret = true;
    }
    else
    {
        // error...
        reply = netReply->errorString();
        ret = false;
    }
    delete netReply;
    return ret;
}
/*****************************************************************************/
void LobbyWindow::slotReturnPressed()
{
    QString message = ui.chatText->text();
    if (message.length() == 0)
    {
        return;
    }
    emit sigEmitMessage(message);
    ui.chatText->clear();
}

//=============================================================================
// End of file LobbyWindow.cpp
//=============================================================================
