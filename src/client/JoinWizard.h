/*=============================================================================
 * TarotClub - JoinWizard.h
 *=============================================================================
 * Wizard pages to join a network server
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
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
private:
   Ui::Page1 ui;
public:
   JoinWizardPage1(QWidget * parent = 0);

   void initializePage();

   QString getIp() { return ui.ipAddress->text(); }
   int getPort() { return ui.portNumber->value(); }
};
/*****************************************************************************/
class JoinWizardPage2 : public QWizardPage
{
private:
   Ui::Page2 ui;
public:
   JoinWizardPage2(QWidget * parent = 0);

   void initializePage();
   void setInfos(QString &txt) { ui.infos->setText(txt); }
   void setSaloons(const QStringList &list) { ui.saloonList->addItems(list); }
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
   void socketError( QAbstractSocket::SocketError code );
};


#endif // JOINWIZARD_H

//=============================================================================
// End of file JoinWizard.h
//=============================================================================
