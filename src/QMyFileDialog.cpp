/*=============================================================================
 * Tarot Club - QMyFileDialog.cpp
 *=============================================================================
 * QMyFileDialog : Fenêtre choix de fichier image avec preview
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

#include "QMyFileDialog.h"
#include <QPixmap>

/*****************************************************************************/
QMyFileDialog::QMyFileDialog(const QString & dirName, QWidget * parent, int taille ) :
	Q3FileDialog(dirName, "", parent, 0, true)
{
	preview = new QFrame(this);
	preview->setFixedSize(100,100);
	preview->setFrameStyle(QFrame::Box | QFrame::NoFrame);

	img = new QLabel( preview, "pixEst" );
	img->setGeometry( QRect( 30, 30, taille, taille ) ); 
	img->setFrameShape( QLabel::Box );
	img->setFrameShadow( QLabel::Plain );
	img->setScaledContents( TRUE );

	addRightWidget(preview);
	setFilter( "Image (*.png)" );
	setMode(Q3FileDialog::ExistingFile);

//	setWFlags(WStyle_Dialog | WType_Modal);

	connect(this, SIGNAL(fileHighlighted(const QString &)), this, SLOT(showImage(const QString &)));

}
/*****************************************************************************/
void QMyFileDialog::showImage(const QString &fichier)
{

	QPixmap plm;
	if( plm.load( fichier ) )
		img->setPixmap( plm );

}

//=============================================================================
// Fin du fichier QMyFileDialog.cpp
//=============================================================================
