/*=============================================================================
 * Tarot Club - NetServerWindow.h
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

#ifndef _SERVEUR_H
#define _SERVEUR_H

// Includes Qt
#include <QDialog>

// Includes locales
#include "ui/ui_NetServerUI.h"
#include "../defines.h"

class NetServerWindow : public QDialog
{
	Q_OBJECT

private:
    Ui::NetServerUI ui;

protected:
	void	closeEvent( QCloseEvent * );

public:
	NetServerWindow( QWidget *parent, Qt::WFlags f=0 );
	void setPlayersList( int nombre, Identity *idents );
    void clear();

signals:
	void sgnlExit();
	void sgnlSendMessage(const QString &message);
    void sgnlStartServer();
    void sgnlStartGame();
    void sgnlDeconnection();

public slots:
	void slotReturnPressed();
	void slotExitGame();
	void slotStartServerBtnClicked();
	void slotStartGameBtnClicked();
    void slotDeconnectionBtnClicked();
	void message(const QString &message);

};

#endif // _SERVEUR_H

//=============================================================================
// Fin du fichier NetServerWindow.h
//=============================================================================
