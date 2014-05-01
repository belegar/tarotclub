/*=============================================================================
 * TarotClub - MessageBoxItem.cpp
 *=============================================================================
 * Central item to show formatted text information
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

#include "MessageBoxItem.h"
#include <QBrush>
#include <QPainter>
#include <QtGlobal>
#include <QPointF>


/*****************************************************************************/
MessageBoxItem::MessageBoxItem()
{
    setBrush(QColor("#808080"));
    setPen(QPen(Qt::white));
    setZValue(3.0);
}
/*****************************************************************************/
void MessageBoxItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Paint with specified color and pen
    painter->setRenderHint(QPainter::Antialiasing);

    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawRoundRect(rect(), (int)(25 * rect().height()
                                         / rect().width()), 25);
}
/*****************************************************************************/

//=============================================================================
// End of file MessageBoxItem.cpp
//=============================================================================
