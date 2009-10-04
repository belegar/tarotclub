/*=============================================================================
 * Tarot Club - Tapis.cpp
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

#include "Tapis.h"
#include <QResizeEvent>
#include <QMouseEvent>

/*****************************************************************************/
Tapis::Tapis( QGraphicsScene *canvas, QWidget *parent )
    : QGraphicsView( canvas, parent )
{
   setSceneRect( canvas->sceneRect() );
   // appel mouseEvent dès que la souris bouge, sans appuis sur les boutons
   viewport()->setMouseTracking(true);
   filter = AUCUN;
}
/*****************************************************************************/
void Tapis::resizeEvent( QResizeEvent *e )
{
   QSize s;
   s = e->size();
   setSceneRect( 0, 0, s.width(), s.height() ); // TODO ?? pas vraiment sûr, à tester en redimensionnant la fenêtre
}
/*****************************************************************************/
void Tapis::mousePressEvent( QMouseEvent *e )
{
   QList<QGraphicsItem *> list;

   emit sgnlViewportClicked();

   if( filter == AUCUN ) {
      return;
   }
   if( filter == JEU ) {
      list = scene()->items(e->pos());
      if ( !list.isEmpty() ) {
         QGraphicsItem *item = list.first();
         if( qgraphicsitem_cast<GfxCard *>(item) ) {
            GfxCard *c = (GfxCard *)item;
            emit sgnlClickCard( c );
         }
      }
   }
}
/*****************************************************************************/
/*
 * Emit a signal each time the cursor goes over a card
 */
void Tapis::mouseMoveEvent( QMouseEvent * e )
{
   QList<QGraphicsItem *> list;

   if( filter == AUCUN ) {
      return;
   }

   list = scene()->items(e->pos());

   if ( !list.isEmpty() ) {
      QGraphicsItem *item = list.first();
      if( qgraphicsitem_cast<GfxCard *>(item) ) {
         GfxCard *c = (GfxCard *)item;
         emit sgnlMoveCursor( c );
      }
   } else {
      setCursorType( ARROW );
   }
}
/*****************************************************************************/
/**
 * n==0 : curseur normal avec une flèche
 * n==1 : curseur interdiction
 */
void Tapis::setCursorType( CursorType t )
{
   if( t == ARROW ) {
      QCursor cu( Qt::ArrowCursor );
      setCursor( cu );
   } else {
      QCursor cu( Qt::ForbiddenCursor );
      setCursor( cu );
   }
}
/*****************************************************************************/
void Tapis::setFilter( Filter f )
{
   filter = f;
}

//=============================================================================
// End of file Tapis.cpp
//=============================================================================
