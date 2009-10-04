/*=============================================================================
 * Tarot Club - AvatarsWindow.h
 *=============================================================================
 * AvatarsWindow : choix d'un avatar
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

#ifndef _AVATARSWINDOW_H
#define _AVATARSWINDOW_H

// Includes Qt
#include <QDialog>

// Includes locales
#include "ui/ui_AvatarsUI.h"

/*****************************************************************************/
class AvatarsWindow : public QDialog
{
	Q_OBJECT

private:
    Ui::Avatars ui;
    QString path;

public:
	AvatarsWindow( QWidget* parent = 0, Qt::WFlags fl = 0 );

	void	listAvatars( const QString &avatarsPath );
    QString selectedFile();

public slots:
    void slotShowImage(QListWidgetItem *item, QListWidgetItem *previous);
};

#endif // _AVATARSWINDOW_H

//=============================================================================
// Fin du fichier AvatarsWindow.h
//=============================================================================
