/*=============================================================================
 * TarotClub - LobbyDock.h
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

#ifndef LOBBY_DOCK_H
#define LOBBY_DOCK_H

#include <QDockWidget>
#include <QTcpSocket>
#include <QMap>
#include "ClientConfig.h"
#include "ui_LobbyUI.h"

/*****************************************************************************/
class LobbyDock : public QDockWidget
{
    Q_OBJECT

public:
    LobbyDock(QWidget *parent);

    void Initialize();
    void SetPlayersNames(const std::map<std::uint32_t, std::string> &players);
    void SetTableStatus(std::uint32_t tableId, bool status);
    void DisconnectedFromServer();
    void SetTables(const std::map<std::string, std::uint32_t> &tableList);
    void SetServersList(const std::vector<ServerInfo> &servers);

signals:
    void sigEmitMessage(const QString &);
    void sigConnect(QString, std::uint16_t);
    void sigDisconnect();
    void sigJoinTable(std::uint32_t);
    void sigQuitTable(std::uint32_t);

public slots:
    void slotMessage(std::string message);
    void slotConnectionFailure();

private slots:
    void slotConnect();
    void slotDisconnect();
    void slotJoin();
    void slotQuit();
    void slotReturnPressed();
    void slotCheckServer();

private:
    Ui::LobbyUI  ui;

    std::map<std::string, std::uint32_t> mTableList;    // key = table name, value = table ID
    std::map<std::uint32_t, std::string> mPlayerList; // key = user name, value = uuid
    std::vector<ServerInfo> mServerList;
    bool mConnected;

    bool RequestHttp(const QString &request, QString &reply);
    bool CheckServer();
    void UpdateServersList();
};


#endif // LOBBY_DOCK_H

//=============================================================================
// End of file LobbyDock.h
//=============================================================================
