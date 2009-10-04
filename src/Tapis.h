/*=============================================================================
 * Tarot Club - Tapis.h
 *=============================================================================
 * CanvasView : visual game contents
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

#ifndef TAPIS_H
#define TAPIS_H

// Qt includes
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>

// Game includes
#include "../defines.h"

/*****************************************************************************/
class GfxCard : public QGraphicsPixmapItem
{

public:

   enum { Type = UserType + 1 };
   int type() const
   {
     // Enable the use of qgraphicsitem_cast with this item.
     return Type;
   }
};
/*****************************************************************************/
class Tapis : public QGraphicsView
{

   Q_OBJECT

private:
   Filter filter;

protected:
   void  mousePressEvent( QMouseEvent *e );
   void  mouseMoveEvent( QMouseEvent * e );
   void  resizeEvent( QResizeEvent * );

public:
    Tapis( QGraphicsScene *canvas, QWidget *parent );

   void  setCursorType( CursorType t );

public slots:  
   void  setFilter( Filter );

signals:
   void  sgnlViewportClicked();
   void  sgnlClickCard( GfxCard * );
   void  sgnlMoveCursor( GfxCard * );

};

#endif // TAPIS_H

//=============================================================================
// End of file Tapis.h
//=============================================================================
