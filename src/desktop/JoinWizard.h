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
#include <ui_JoinWizardPage1.h>
#include <ui_JoinWizardPage2.h>

/*****************************************************************************/
class JoinWizardPage1 : public QWizardPage
{
    Q_OBJECT

private:
    Ui::Page1 ui;
public:
    JoinWizardPage1(QWidget *parent = 0);

    void initializePage();

    QString GetIp()
    {
        return ui.ipAddress->text();
    }
    int GetPort()
    {
        return ui.portNumber->value();
    }
};
/*****************************************************************************/
class JoinWizardPage2 : public QWizardPage
{
    Q_OBJECT

private:
    Ui::Page2 ui;

signals:
    void sigRoomSelected(const QString &room);
    void sigTableSelected(const QString &room, const QString &table);

public:
    JoinWizardPage2(QWidget *parent = 0);

    void initializePage();
    void setInfos(QString &txt)
    {
        ui.infos->setText(txt);
    }
    void SetSaloons(const QStringList &list)
    {
        ui.saloonList->clear();
        ui.saloonList->addItems(list);
    }

    void SetTables(const QStringList &list)
    {
        ui.tableList->clear();
        ui.tableList->addItems(list);
    }

    bool isComplete() const;
    void Ready();

public slots:
    void slotRoomSelected(QListWidgetItem *item);
    void slotTableSelected(QListWidgetItem *item);
};
/*****************************************************************************/
class JoinWizard : public QWizard
{
    Q_OBJECT

public:
    enum { Page_Server, Page_Lobby };

    JoinWizard(QWidget *parent);

    struct Connection
    {
        QString ip;
        quint16 port;
        bool isValid;
    };

    Connection GetTableConnection();

public slots:
    void slotPageChanged(int id);
    void slotRoomClicked(const QString &room);
    void slotTableClicked(const QString &room, const QString &table);

    // socket
    void socketReadData();
    void socketConnected();
    void socketHostFound();
    void socketClosed();
    void socketError(QAbstractSocket::SocketError code);

private:
    JoinWizardPage1 *page1;
    JoinWizardPage2 *page2;

    QTcpSocket  socket;
    Connection  selectedTable;
};


#endif // JOINWIZARD_H

//=============================================================================
// End of file JoinWizard.h
//=============================================================================
