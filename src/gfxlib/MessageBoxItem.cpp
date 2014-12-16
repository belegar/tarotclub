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
    : RectBase(false)
{
    mText.setParentItem(this);
    mText.setZValue(4.0);

    setZValue(3.0);
}
/*****************************************************************************/
void MessageBoxItem::SetText(const QString &text, const QSize &size)
{
    qreal x = mBorder.x() + ((mBorder.width() - size.width()) / 2);
    qreal y = mBorder.y() + ((mBorder.height() - size.height()) / 2);

    mText.setPos(x + 10, y + 10);
    mText.setTextWidth(size.width() - 20);
    setRect(x, y, size.width(), size.height());

    mText.setHtml(text);
    mText.setDefaultTextColor(QColor(Qt::white));
}


//=============================================================================
// End of file MessageBoxItem.cpp
//=============================================================================
