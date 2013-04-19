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
 * @brief Card shadow item to show the positions of cards on the play board
 */
class CardShadow : public QGraphicsRectItem
{

public:
   CardShadow(QRectF &pos, QGraphicsScene *canvas);
   void paint ( QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

};


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
   TextBox(const QPointF &pos, QGraphicsScene *canvas);
   void paint ( QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

   void move( int x, int y ) { setPos( x, y ); }
   void setText(const QString &s) { text = s; text.truncate(10); }
   void setPenWidth(int w) { penWidth = w; }
   void setPenColor(Qt::GlobalColor c) { penColor = c; }
   void setFillColor(QColor &c) { fillColor = c; }
};

/*****************************************************************************/
/**
 * Advanced graphics box item with icon
 */
class PlayerBox : public TextBox
{
private:
   QGraphicsPixmapItem		*avatar;

public:
   PlayerBox(const QPointF &pos, QGraphicsScene *canvas);

   void selectPlayer(bool selected);
   void highlightPlayer(bool highlighted);
   void setAvatar(const QString & avatar);
   void enableAvatar(bool enable);

};

#endif // _TEXT_BOX_H

//=============================================================================
// End of file TextBox.h
//=============================================================================
