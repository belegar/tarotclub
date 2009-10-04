/*=============================================================================
 * Tarot Club - NetServerWindow.cpp
 *=============================================================================
 * NetServerWindow : Gère les parties en réseau du côté serveur
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

#include "NetServerWindow.h"

/*****************************************************************************/
NetServerWindow::NetServerWindow( QWidget *parent, Qt::WFlags f )
	: QDialog( parent, f )
{
    ui.setupUi(this);
    
	connect( ui.textLine,SIGNAL( returnPressed() ), this, SLOT(slotReturnPressed()) );
	connect( ui.btnExitGame,SIGNAL( clicked() ), this, SLOT(slotExitGame()) );
    connect( ui.btnStartServer, SIGNAL(clicked()), this, SLOT(slotStartServerBtnClicked()));
    connect( ui.btnStartGame, SIGNAL(clicked()), this, SLOT(slotStartGameBtnClicked()));
    connect( ui.btnDeconnection, SIGNAL(clicked()), this, SLOT(slotDeconnectionBtnClicked()));
    
}
/*****************************************************************************/
void NetServerWindow::clear()
{
    ui.playersList->clear();
    ui.chatWindow->clear();
}
/*****************************************************************************/
void NetServerWindow::slotStartGameBtnClicked()
{
    emit sgnlStartGame();
}
/*****************************************************************************/
void NetServerWindow::slotStartServerBtnClicked()
{
    emit sgnlStartServer();
}
/*****************************************************************************/
void NetServerWindow::slotDeconnectionBtnClicked()
{
    emit sgnlDeconnection();
}
/*****************************************************************************/
void NetServerWindow::setPlayersList( int nombre, Identity *idents )
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
void NetServerWindow::message(const QString &message)
{
	ui.chatWindow->append( message );
}
/*****************************************************************************/
void NetServerWindow::closeEvent( QCloseEvent * e )
{
	emit sgnlExit();
}
/*****************************************************************************/
void NetServerWindow::slotExitGame()
{
	emit sgnlExit();
}
/*****************************************************************************/
void NetServerWindow::slotReturnPressed()
{
	QString message = ui.textLine->text();
	if( message.length() == 0 )
		return;
	
	emit sgnlSendMessage(message);
	ui.textLine->clear();
}

//=============================================================================
// Fin du fichier NetServerWindow.cpp
//=============================================================================
