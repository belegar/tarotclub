/*=============================================================================
 * TarotClub - JoinWizard.h
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

#ifndef JOINWIZARD_H
#define JOINWIZARD_H

#include <QWizard>
#include <QTcpSocket>
#include "ui_LobbyUI.h"

/*****************************************************************************/
class LobbyWindow : public QDialog
{
    Q_OBJECT

public:

    struct Connection
    {
        QString ip;
        quint16 port;
        bool isValid;
    };

    LobbyWindow(QWidget *parent);

    Connection GetTableConnection();

public slots:
    void slotRoomSelected(QListWidgetItem *item);
    void slotTableSelected(QListWidgetItem *item);

    void slotConnect();
    void slotJoin();
    void slotClose();

    // socket
    void socketReadData();
    void socketConnected();
    void socketHostFound();
    void socketClosed();
    void socketError(QAbstractSocket::SocketError code);

private:
    Ui::LobbyUI  ui;

    QTcpSocket  socket;
    Connection  selectedTable;
};


#endif // JOINWIZARD_H

//=============================================================================
// End of file JoinWizard.h
//=============================================================================
