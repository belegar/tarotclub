/*=============================================================================
 * TarotClub - LobbyWindow.h
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

#ifndef LOBBY_WINDOW_H
#define LOBBY_WINDOW_H

#include <QTcpSocket>
#include <QMap>
#include "ui_LobbyUI.h"

/*****************************************************************************/
class LobbyWindow : public QDialog
{
    Q_OBJECT

public:
    LobbyWindow(QWidget *parent);

    void Initialize();

signals:
    void sigConnect(QString, std::uint16_t);
    void sigJoinTable(std::uint32_t);

public slots:
    void slotMessage(std::string message);

private slots:
    void slotConnect();
    void slotJoin();
    void slotClose();

private:
    Ui::LobbyUI  ui;

    QMap<QString, std::uint32_t> mTableList;
    bool RequestHttp(const QString &request, QString &reply);
};


#endif // LOBBY_WINDOW_H

//=============================================================================
// End of file LobbyWindow.h
//=============================================================================
