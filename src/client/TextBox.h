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

typedef struct {
   int x, y;
} Coord;

/*****************************************************************************/
/**
 * Rectangle box + text inside, for bids and player nicknames
 */
class TextBox : public QGraphicsRectItem
{

private:
   QString  text;
   int penWidth;
   QColor penColor;
   QColor fillColor;

public:
   TextBox(Coord c, QGraphicsScene *canvas);
   void paint ( QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

   void move( int x, int y ) { setPos( x, y ); }
   void setText( QString s ) {
      s.truncate( 10 );
      text = s;
   }
   void setPenWidth(int w) { penWidth = w; }
   void setPenColor(QColor c) { penColor = c; }
   void setFillColor(QColor c) { fillColor = c; }
};

/*****************************************************************************/
/**
 * Widget avancé avec des icônes
 */
class PlayerBox : public TextBox
{

private:
   QGraphicsPixmapItem		*avatar;

public:
   PlayerBox(Coord c, QGraphicsScene *canvas);

   void selectPlayer(bool selected);
   void highlightPlayer(bool highlighted);
   void setAvatar( const QString & );
   void enableAvatar( bool );

};

#endif // _TEXT_BOX_H

//=============================================================================
// End of file TextBox.h
//=============================================================================
