/*=============================================================================
 * Tarot Club - TextBox.cpp
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

#include "TextBox.h"
#include <QFile>
#include <QRectF>
#include <QPixmap>

/*****************************************************************************/
TextBox::TextBox(int x, int y, QBrush couleur , QGraphicsScene *canvas)
{
	boite = new QGraphicsRectItem( x, y, 115, 30, 0, canvas);
	boite->setBrush( couleur );
	boite->setZValue( 0 );

	texte = new QGraphicsSimpleTextItem(0, canvas);
	texte->setText("");
	texte->setFont( QFont( "times", 14 ) );
	texte->setZValue( 1 );

   hide();
}
/*****************************************************************************/
void TextBox::setText( QString mot )
{
	mot.truncate( 10 ); // on se limite à 10 signes pour le nom
	texte->setText( mot );

	moveText();
}
/*****************************************************************************/
/**
 * On centre le texte au milieu de la boîte
 */
void TextBox::moveText()
{
	QRectF rect, rectText;
	int x, y;
	int largeur, n;

	rect = boite->rect();
	x = rect.x();
	y = rect.y();

	rectText = texte->boundingRect();
	largeur = rectText.width ();

	n = (int)(rect.width()/2)  - (int)(largeur/2);

	texte->setPos(x+n,y+5);
}
/*****************************************************************************/
void TextBox::hide()
{
	boite->hide();
	texte->hide();
}
/*****************************************************************************/
void TextBox::show()
{
	boite->show();
	texte->show();
}
/*****************************************************************************/
int TextBox::x()
{
	return (int)(boite->rect().x());
}
/*****************************************************************************/
int TextBox::y()
{
	return (int)(boite->rect().y());
}
/*****************************************************************************/
void TextBox::move( int x, int y )
{
	boite->setPos( x, y );
	moveText();
}
/*****************************************************************************/

/*			*			*			*			*			*			*/

/*****************************************************************************/
PlayerBox::PlayerBox(int x, int y, QBrush couleur , QGraphicsScene *canvas )
	: TextBox( x, y, couleur, canvas)
{
   QRectF rect = boite->rect();

	selection.setColor( QColor("red") );
	selection.setWidth(3);

	// On initialise avec les images par défaut
	avatar = new QGraphicsPixmapItem( QPixmap( ":/images/vide.png"), 0, canvas);
	avatar->hide();

	// On les positionne à l'écran
	avatar->setPos( rect.x() + rect.width() + 10, rect.y()-5 );
}
/*****************************************************************************/
void PlayerBox::setAvatar( const QString &av )
{
	QFile f( av );

	if( f.exists() == false )
		return;

   avatar->setPixmap( QPixmap(av) );
}
/*****************************************************************************/
void PlayerBox::enableAvatar( bool e )
{
	if( e == true )
		avatar->show();
	else
		avatar->hide();
}
/*****************************************************************************/
void PlayerBox::hide()
{
	boite->hide();
	texte->hide();
}
/*****************************************************************************/
void PlayerBox::show()
{
	boite->show();
	texte->show();
}
/*****************************************************************************/
void PlayerBox::setCouleur( QBrush couleur )
{
	boite->setBrush( couleur );
}
/*****************************************************************************/
void PlayerBox::setPen( int p )
{
	if( p == 1 )
		boite->setPen( selection );
	else
		boite->setPen( normal );
	
}

//=============================================================================
// End of file TextBox.cpp
//=============================================================================
