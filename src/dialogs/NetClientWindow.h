/*=============================================================================
 * Tarot Club - NetClientWindow.h
 *=============================================================================
 * NetClientWindow : Gère les parties en réseau du côté client
 *=============================================================================
 * Tarot Club est un jeu de Tarot français
 * Copyright (C) 2003-2005  Anthony Rabine
 * anthony@ooso.org
 * http://tarotclub.ooso.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *=============================================================================
 */

#ifndef NETCLIENTWINDOW_H
#define NETCLIENTWINDOW_H

// Includes Qt
#include <QDialog>

// Includes locales
#include "ui/ui_NetClientUI.h"
#include "../defines.h"

/*****************************************************************************/
class NetClientWindow : public QDialog
{
   Q_OBJECT

private:
   Ui::NetClientUI ui;

protected:
   void closeEvent( QCloseEvent * );

public:
   NetClientWindow( QWidget *parent, Qt::WFlags f=0 );
   void message(const QString &message);
   void setPlayersList( int nombre, Identity *idents );
   void clearPlayersList();
   void clear();

signals:
   void sgnlExit();
   void sgnlSendMessage(const QString &message);
   void sgnlConnection(const QString &ip_address);
   void sgnlDeconnection();

public slots:
   void slotExitGame();
   void slotReturnPressed();
   void slotConnectionBtnClicked();
   void slotDeconnectionBtnClicked();
};

#endif // NETCLIENTWINDOW_H

//=============================================================================
// Fin du fichier NetServerWindow.h
//=============================================================================
