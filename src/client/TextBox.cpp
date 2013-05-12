/*=============================================================================
 * TarotClub - TextBox.cpp
 *=============================================================================
 * Two text box classes to show names and bids on canvas
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#include "TextBox.h"
#include "../defines.h"
#include <QFile>
#include <QtGui>

/*****************************************************************************/
CardShadow::CardShadow(QRectF &pos, QGraphicsScene *canvas)
    : QGraphicsRectItem(pos)
{
    canvas->addItem(this);
}
/*****************************************************************************/
void CardShadow::paint ( QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
    Q_UNUSED( option );
    Q_UNUSED( widget );

    // Paint with specified color and pen
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(QBrush(QColor(149, 149, 149, 127)));
    painter->setPen(Qt::NoPen);
    painter->drawRoundRect(rect(), 25, 25);

}

/*****************************************************************************/
/*            *            *           *            *           *            */
/*****************************************************************************/


/*****************************************************************************/
TextBox::TextBox(const QPointF &pos, QGraphicsScene *canvas)
    : QGraphicsRectItem( pos.x(), pos.y(), TEXT_BOX_WIDTH, TEXT_BOX_HEIGHT)
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


PlayerBox::PlayerBox(const QPointF &pos, QGraphicsScene *canvas )
    : TextBox( pos, canvas)
{
    QColor color(255, 255, 255, 127);
    setFillColor(color); // transparent

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
    {
        return;
    }

    avatar->setPixmap( QPixmap(av) );
}
/*****************************************************************************/
void PlayerBox::enableAvatar(bool enable)
{
    if( enable == true )
    {
        avatar->show();
    }
    else
    {
        avatar->hide();
    }
}
/*****************************************************************************/
void PlayerBox::selectPlayer(bool selected)
{
    Qt::GlobalColor color;

    if (selected == true)
    {
        setPenWidth(2);
        color = Qt::red;
    }
    else
    {
        setPenWidth(1);
        color = Qt::black;
    }
    setPenColor(color);
    update();
}
/*****************************************************************************/
void PlayerBox::highlightPlayer(bool highlighted)
{
    QColor color;

    if (highlighted == true)
    {
        color = Qt::yellow;
    }
    else
    {
        color.setRgb(255, 255, 255, 127); // transparent
    }

    setFillColor(color);
    update();
}

//=============================================================================
// End of file TextBox.cpp
//=============================================================================
