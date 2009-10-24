/*=============================================================================
 * Tarot Club - QMyFileDialog.cpp
 *=============================================================================
 * QMyFileDialog : fenêtre choix de fichier image avec preview
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

#ifndef QMYFILEDIALOG_H
#define QMYFILEDIALOG_H

// Includes Qt
#include <QFileDialog>
#include <QFrame>
#include <QLabel>

/*****************************************************************************/

class QMyFileDialog : public Q3FileDialog
{

	Q_OBJECT

private:
	QFrame *preview;
	QLabel *img;

public:

	QMyFileDialog(const QString &dirName, QWidget * parent, int );

public slots:
	void showImage(const QString &);

};

#endif // QMYFILEDIALOG_H

//=============================================================================
// Fin du fichier QMyFileDialog.h
//=============================================================================
