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
#include "JoinWizard.h"
#include "ServerConfig.h"

/*****************************************************************************/
JoinWizardPage1::JoinWizardPage1(QWidget *parent) : QWizardPage(parent)
{
    ui.setupUi(this);
    setTitle(trUtf8("Connection to a TarotClub server"));
    setSubTitle(trUtf8("A game server can be created using a TarotClub cient or a dedicated server."));
}
/*****************************************************************************/
void JoinWizardPage1::initializePage()
{
    ui.ipAddress->setCursorPosition(0);
#ifndef QT_NO_DEBUG
    ui.ipAddress->setText("127.0.0.1");
#endif
    ui.portNumber->setValue(4242);
}
/*****************************************************************************/



/*****************************************************************************/
JoinWizardPage2::JoinWizardPage2(QWidget *parent) : QWizardPage(parent)
{
    ui.setupUi(this);
    setTitle(trUtf8("Connection to a TarotClub server"));
    setSubTitle(trUtf8("Choose your room and game table."));

    connect(ui.saloonList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotRoomSelected(QListWidgetItem *)));
    connect(ui.tableList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotTableSelected(QListWidgetItem *)));
}
/*****************************************************************************/
void JoinWizardPage2::initializePage()
{

}
/*****************************************************************************/
void JoinWizardPage2::slotRoomSelected(QListWidgetItem *item)
{
    emit sigRoomSelected(item->text());
}
/*****************************************************************************/
void JoinWizardPage2::slotTableSelected(QListWidgetItem *item)
{
    emit sigTableSelected(ui.saloonList->selectedItems().at(0)->text(), item->text());
}
/*****************************************************************************/
void JoinWizardPage2::Ready()
{
    emit completeChanged();
}
/*****************************************************************************/
bool JoinWizardPage2::isComplete() const
{
    if ((ui.saloonList->selectedItems().size() == 1) &&
        (ui.tableList->selectedItems().size() == 1))
    {
        return true;
    }
    return false;
}
/*****************************************************************************/



/*****************************************************************************/
JoinWizard::JoinWizard(QWidget *parent) : QWizard(parent)
{
    page1 = new JoinWizardPage1(this);
    page2 = new JoinWizardPage2(this);

    selectedTable.isValid = false;

    setPage(Page_Server, page1);
    setPage(Page_Lobby, page2);

    setButtonText(QWizard::FinishButton, "Connect");
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":images/wizard.png"));
    setWizardStyle(QWizard::ModernStyle);

    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(slotPageChanged(int)));
    connect(page2, SIGNAL(sigRoomSelected(const QString &)), this, SLOT(slotRoomClicked(const QString &)));
    connect(page2, SIGNAL(sigTableSelected(const QString &, const QString &)), this, SLOT(slotTableClicked(const QString &, const QString &)));

    // socket events
    connect(&socket, SIGNAL(readyRead()), this, SLOT(socketReadData()));
    connect(&socket, SIGNAL(disconnected()), this, SLOT(socketClosed()));
    connect(&socket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(&socket, SIGNAL(hostFound()), this, SLOT(socketHostFound()));
}
/*****************************************************************************/
JoinWizard::Connection JoinWizard::GetTableConnection()
{
    return selectedTable;
}
/*****************************************************************************/
void JoinWizard::slotPageChanged(int id)
{
    if (id == Page_Lobby)
    {
        socket.close();
        socket.connectToHost(page1->GetIp(), page1->GetPort());
    }
    selectedTable.isValid = false;
}
/*****************************************************************************/
void JoinWizard::slotRoomClicked(const QString &room)
{
    selectedTable.isValid = false;
    // get list of tables in this room
    QTextStream os(&socket);
    os << "GET:TABLES:" << room;
    os << "\n";
    os.flush();
}
/*****************************************************************************/
void JoinWizard::slotTableClicked(const QString &room, const QString &table)
{
    selectedTable.isValid = false;
    // get the tcp/ip port of this table
    QTextStream os(&socket);
    os << "GET:PORT:" << room << "," << table;
    os << "\n";
    os.flush();
}
/*****************************************************************************/
void JoinWizard::socketReadData()
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
            page2->SetSaloons(list);
        }
        else if (tokens[0] == "TABLES")
        {
            QStringList list = tokens[1].split(',');
            page2->SetTables(list);
        }
        else if (tokens[0] == "PORT")
        {
            selectedTable.port = tokens[1].toUInt();
            selectedTable.ip = page1->GetIp();
            selectedTable.isValid = true;
            page2->Ready();
        }
    }
}
/*****************************************************************************/
void JoinWizard::socketConnected()
{
    QString txt = trUtf8("Connected to the server.");
    page2->setInfos(txt);

    // get list of playing rooms
    QTextStream os(&socket);
    os << "GET:INFOS\n";
    os.flush();
}
/*****************************************************************************/
void JoinWizard::socketHostFound()
{
    QString txt = trUtf8("Connection to the server ....");
    page2->setInfos(txt);
}
/*****************************************************************************/
void JoinWizard::socketClosed()
{
    QString txt = trUtf8("The connection has been closed by the server.");
    page2->setInfos(txt);
}
/*****************************************************************************/
void JoinWizard::socketError(QAbstractSocket::SocketError code)
{
    QString txt = trUtf8("Socket error - code: ") + QString().setNum((int)code);
    page2->setInfos(txt);
}

//=============================================================================
// End of file JoinWizard.cpp
//=============================================================================
