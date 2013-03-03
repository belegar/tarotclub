/*=============================================================================
 * TarotClub - TextBox.cpp
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

#include "TextBox.h"
#include "../defines.h"
#include <QFile>
#include <QtGui>


/*****************************************************************************/
TextBox::TextBox(Coord c, QGraphicsScene *canvas)
   : QGraphicsRectItem( c.x, c.y, TEXT_BOX_WIDTH, TEXT_BOX_HEIGHT)
{
   penWidth = 1;
   penColor = Qt::black;
   fillColor = Qt::red;
   hide();
   canvas->addItem(this);

}
/*****************************************************************************/
void TextBox::paint ( QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
   Q_UNUSED( option );
   Q_UNUSED( widget );

   // Paint with specified color and pen
   painter->setRenderHint(QPainter::Antialiasing);
   painter->setPen(QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
   QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());
   gradient.setColorAt(0.0, Qt::transparent);
   gradient.setColorAt(1.0, fillColor);
   painter->setBrush(gradient);
   painter->drawRoundRect(rect(), (int) (25 * rect().height()
                                           / rect().width()), 25);

   // Text inside the box
   painter->save();
   QFont font = painter->font();
   font.setBold(true);
   painter->setPen(Qt::black);
   painter->setFont(font);
   painter->drawText(rect(), Qt::AlignCenter, text);
   painter->restore();
}


/*****************************************************************************/
/*            *            *           *            *           *            */
/*****************************************************************************/


PlayerBox::PlayerBox(Coord c, QGraphicsScene *canvas )
   : TextBox( c, canvas)
{
   setFillColor(QColor(255, 255, 255, 127)); // transparent

   // Init with defaut image
   avatar = new QGraphicsPixmapItem( QPixmap( ":/images/vide.png"));
   avatar->hide();

   // we set it to the right place
   avatar->setPos( rect().x() + rect().width() + 10, rect().y()-5 );

   // we add the item to the scene
   canvas->addItem(avatar);
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
void PlayerBox::selectPlayer(bool selected)
{
   if (selected == true) {
      setPenWidth(2);
      setPenColor(Qt::red);
   } else {
      setPenWidth(1);
      setPenColor(Qt::black);
   }
   update();
}
/*****************************************************************************/
void PlayerBox::highlightPlayer(bool highlighted)
{
   if (highlighted == true) {
      setFillColor(Qt::yellow);
   } else {
      setFillColor(QColor(255, 255, 255, 127)); // transparent
   }
   update();
}

//=============================================================================
// End of file TextBox.cpp
//=============================================================================
