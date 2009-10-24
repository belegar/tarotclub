/*=============================================================================
 * Tarot Club - NetClientWindow.cpp
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

#include "NetClientWindow.h"

/*****************************************************************************/
NetClientWindow::NetClientWindow( QWidget *parent, Qt::WFlags f )
   : QDialog( parent, f )
{
    ui.setupUi(this);

   connect( ui.btnConnect,SIGNAL( clicked() ), this, SLOT(slotConnectionBtnClicked()) );
   connect( ui.btnDeconnection,SIGNAL( clicked() ), this, SLOT(slotDeconnectionBtnClicked()) );
   connect( ui.textLine, SIGNAL( returnPressed() ), this, SLOT(slotReturnPressed()) );
   connect( ui.btnExitGame,SIGNAL( clicked() ), this, SLOT(slotExitGame()) );

#ifndef QT_NO_DEBUG
   ui.ipAddress->setText("127.0.0.1");
#endif

}
/*****************************************************************************/
void NetClientWindow::clear()
{
    ui.playersList->clear();
    ui.chatWindow->clear();
}
/*****************************************************************************/
void NetClientWindow::slotConnectionBtnClicked()
{
   emit sgnlConnection( ui.ipAddress->text() );
}
/*****************************************************************************/
void NetClientWindow::slotDeconnectionBtnClicked()
{
    emit sgnlDeconnection();
}
/*****************************************************************************/
void NetClientWindow::clearPlayersList()
{
    ui.playersList->clear();
}
/*****************************************************************************/
void NetClientWindow::setPlayersList( int nombre, Identity *idents )
{
   QString server;
   QStringList nicks;

   for( int i=0; i<nombre; i++ )
      nicks += idents[i].name;

    ui.playersList->clear();
   // On marque le premier joueur comme étant le serveur
   ui.playersList->insertStringList( nicks );
   server = ui.playersList->text(0);
   server.prepend( '@' );
   ui.playersList->changeItem( server, 0 );
}
/*****************************************************************************/
void NetClientWindow::closeEvent( QCloseEvent * e )
{
   emit sgnlExit();
}
/*****************************************************************************/
void NetClientWindow::slotExitGame()
{
   emit sgnlExit();
}
/*****************************************************************************/
void NetClientWindow::slotReturnPressed()
{
   QString message = ui.textLine->text();
   if( message.length() == 0 )
      return;
   
   emit sgnlSendMessage(message);
   ui.textLine->clear();
}
/*****************************************************************************/
void NetClientWindow::message(const QString &message)
{
   ui.chatWindow->append( message );
}

//=============================================================================
// Fin du fichier NetClientWindow.cpp
//=============================================================================
