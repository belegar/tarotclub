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

    QString getIp()
    {
        return ui.ipAddress->text();
    }
    int getPort()
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
public:
    JoinWizardPage2(QWidget *parent = 0);

    void initializePage();
    void setInfos(QString &txt)
    {
        ui.infos->setText(txt);
    }
    void setSaloons(const QStringList &list)
    {
        ui.saloonList->addItems(list);
    }
};
/*****************************************************************************/
class JoinWizard : public QWizard
{
    Q_OBJECT

private:
    JoinWizardPage1 *page1;
    JoinWizardPage2 *page2;

    QTcpSocket  socket;

public:
    enum { Page_Server, Page_Lobby };

    JoinWizard(QWidget *parent);

public slots:
    void slotServerConnection(int id);
    void socketReadData();
    void socketConnected();
    void socketHostFound();
    void socketClosed();
    void socketError(QAbstractSocket::SocketError code);
};


#endif // JOINWIZARD_H

//=============================================================================
// End of file JoinWizard.h
//=============================================================================
