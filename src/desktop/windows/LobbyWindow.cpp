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

    connect(ui.chatText, &QLineEdit::returnPressed, this, &LobbyWindow::slotReturnPressed);

    Initialize();

#if TAROT_DEBUG
    ui.serverList->addItem("192.168.1.30");
    ui.serverList->addItem("127.0.0.1");
#endif
}
/*****************************************************************************/
void LobbyWindow::slotConnect()
{
    ui.infoLabel->setText(trUtf8("Connecting ..."));
    QListWidgetItem *item = ui.serverList->currentItem();
    if (item != NULL)
    {
        QHostInfo info = QHostInfo::fromName(item->text());
        QString gamePort = ui.gameTcpPort->text();
        QString webPort = ui.webTcpPort->text();

        if (info.error() == QHostInfo::NoError)
        {
            QList<QHostAddress>	addresses = info.addresses();
            // Try each IP address to find any suitable server
            for (QList<QHostAddress>::iterator iter = addresses.begin(); iter != addresses.end(); ++iter)
            {
                QString reply;
                QString request = QString("http://") + iter->toString() + ":" + webPort;
                if (RequestHttp(request + "/tables", reply))
                {
                    // found something interesting
                    QStringList strList = reply.split(",");
                    ui.tableList->clear();
                    mTableList.clear();

                    QRegExp rex("^(\\w+)\\((\\d+)\\)");
                    for (int i = 0; i < strList.size(); i++)
                    {
                        int pos = rex.indexIn(strList[i]);
                        if ((pos > -1) && (rex.captureCount() == 2))
                        {
                            QString name = rex.cap(1);
                            std::uint32_t id = rex.cap(2).toULong();
                            mTableList[name] = id;

                            // Update the table list
                            ui.tableList->addItem(name);
                        }
                    }

                    // It seems that we have found a valid server, try to connect
                    emit sigConnect(iter->toString(), (std::uint16_t)gamePort.toUShort());
                }
                else
                {
                    ui.textArea->append(reply);
                }
            }
        }
    }
}
/*****************************************************************************/
void LobbyWindow::slotDisconnect()
{
    ui.infoLabel->setText(trUtf8("Not connected."));
    mTableList.clear();
    ui.tableList->clear();
    ui.playerList->clear();
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
void LobbyWindow::slotJoin()
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
void LobbyWindow::slotQuit()
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
bool LobbyWindow::CheckServer()
{
    bool ret = false;
    QListWidgetItem *item = ui.serverList->currentItem();
    if (item != NULL)
    {
        QHostInfo info = QHostInfo::fromName(item->text());
        QString webPort = ui.webTcpPort->text();

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
    QString txt = trUtf8("Not connected.");
    ui.infoLabel->setText(txt);
    ui.tableList->clear();
    ui.playerList->clear();
    ui.textArea->clear();
    ui.serverStatus->setText("-");
    ui.gameTcpPort->setValue(4269);
    ui.webTcpPort->setValue(8080);
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
