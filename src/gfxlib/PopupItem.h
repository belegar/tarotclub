/*=============================================================================
 * TarotClub - PopupItem.h
 *=============================================================================
 * Central item to show cards (dog, handle) on the middle of the screen
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

#ifndef POPUP_ITEM_H_
#define POPUP_ITEM_H_

#include <QGraphicsRectItem>
#include <QRectF>

/*****************************************************************************/
class PopupItem : public QGraphicsRectItem
{
public:
    PopupItem();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    // Helpers
    void DrawItems(const QList<QGraphicsItem *> &items);
    void HideAll();

    // Setters
    void SetSizes(const QRectF &border, const QRectF &card);

private:
    QList<QGraphicsItem *> mItems;
    QRectF mBorder;
    QRectF mCardSize;

};

#endif // POPUP_ITEM_H_

//=============================================================================
// End of file PopupItem.h
//=============================================================================
