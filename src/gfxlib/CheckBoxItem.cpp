/*=============================================================================
 * TarotClub - CheckBoxItem.cpp
 *=============================================================================
 * Graphical clickable check box item
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

// Qt includes
#include <QBrush>
#include <QPainter>
#include <QtGlobal>

// Game includes
#include "CheckBoxItem.h"

/*****************************************************************************/
CheckBoxItem::CheckBoxItem(QGraphicsItem *parent, std::uint32_t space)
    :  QGraphicsItem(parent)
    , status(false)
    , tick(":images/tick.svg", this)
    , text(this)
    , square(this)
    , mSpace(space)
{
    tick.setPos(0, 0);
    tick.setScale(0.1);
    tick.hide();

    square.setRect(0, 10, 15, 15);
    QPen pen = square.pen();
    pen.setWidth(2);
    square.setPen(pen);

    text.setText("Slam");
    QFont font = text.font();
    font.setBold(true);
    text.setFont(font);
    text.setPos(mSpace + square.boundingRect().width(), 10);
}
/*****************************************************************************/
int CheckBoxItem::type() const
{
    // Enable the use of qgraphicsitem_cast with this item.
    return Type;
}
/*****************************************************************************/
void CheckBoxItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(widget)

}
/*****************************************************************************/
QRectF CheckBoxItem::boundingRect() const
{
    qreal width = square.boundingRect().width() + text.boundingRect().width() + mSpace;
    qreal height = qMax(square.boundingRect().height(), text.boundingRect().height()) + 10;

    QRectF size(0, 0, width, height);
    return size;
}
/*****************************************************************************/
void CheckBoxItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    (void)event;
    if (status)
    {
        status = false;
        tick.hide();
    }
    else
    {
        status = true;
        tick.show();
    }
}

//=============================================================================
// End of file CheckBoxItem.cpp
//=============================================================================
