/*=============================================================================
 * TarotClub - TextBox.h
 *=============================================================================
 * Two text box classes to show names and bids on canvas
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

#ifndef _TEXT_BOX_H
#define _TEXT_BOX_H

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
   int		x();
   int		y();
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

#endif // _TEXT_BOX_H

//=============================================================================
// End of file TextBox.h
//=============================================================================
