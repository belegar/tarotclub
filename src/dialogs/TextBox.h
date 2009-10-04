/*=============================================================================
 * Tarot Club - TextBox.h
 *=============================================================================
 * Two text box classes to show names and bids on canvas
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

// Qt includes
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsScene>

/*****************************************************************************/
/**
 * Rectangle box + text inside, for bids
 */
class TextBox
{

protected:

	QGraphicsRectItem	         *boite;
	QGraphicsSimpleTextItem		*texte;

public:

	TextBox(int , int , QBrush, QGraphicsScene *);

	void		show();
	void		hide();
	void		moveText();
	void		move( int , int );
	int			x();
	int			y();
	void		setText( QString );
};

/*****************************************************************************/
/**
 * Widget avancé avec des icônes
 */
class PlayerBox : public TextBox
{

private:
	QGraphicsPixmapItem		*avatar;

	QPen				selection;	// contour grossi et en rouge
	QPen				normal;		// contour normal, fin et en noir

public:

	PlayerBox(int , int , QBrush, QGraphicsScene * );

	void		show();
	void		hide();
	void		setCouleur( QBrush );
	void		setPen( int );
	void		setAvatar( const QString & );
	void		enableAvatar( bool );
//	void		afficheErreur( int );
	
};

//=============================================================================
// End of file TextBox.h
//=============================================================================
