/*=============================================================================
 * TarotClub - MessageBoxItem.h
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

#ifndef MESSAGE_BOX_ITEM_H_
#define MESSAGE_BOX_ITEM_H_

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QRectF>

/*****************************************************************************/
class MessageBoxItem : public QGraphicsRectItem
{
public:
    MessageBoxItem();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void SetText(const QString &text, const QSize &size);
    void SetBorder(const QRectF &border)
    {
        mBorder = border;
    }

private:
    QGraphicsTextItem mText;
    QRectF mBorder;
};

#endif // MESSAGE_BOX_ITEM_H_

//=============================================================================
// End of file MessageBoxItem.h
//=============================================================================
