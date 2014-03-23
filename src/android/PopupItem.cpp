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

#include "PopupItem.h"
#include <QBrush>
#include <QPainter>
#include <QtGlobal>
#include <QPointF>

static const qreal STEP_X = 40.0f;

/*****************************************************************************/
PopupItem::PopupItem()
{
    setBrush(QColor("#808080"));
    setPen(QPen(Qt::white));
    setZValue(3.0);
}
/*****************************************************************************/
void PopupItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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
void PopupItem::DrawItems(const QList<QGraphicsItem *> &items)
{
    qreal width = STEP_X * (items.size() - 1) + mCardSize.width() + 20; // add 10 px on each sides
    qreal height = mCardSize.height() + 20; // add 10 px on top/bottom
    qreal x = mBorder.x() + ((mBorder.width() - width) / 2);
    qreal y = mBorder.y() + ((mBorder.height() - height) / 2);

    setRect(x, y, width, height);

    qreal cardPosX = x + 10.0f;
    qreal cardPosY = y + 10.0f;
    mItems = items;

    for (int i = 0; i < items.size(); i++)
    {
        items.at(i)->setPos(cardPosX + i * STEP_X, cardPosY);
        items.at(i)->setZValue(4 + i);
        items.at(i)->show();
    }
    update();
}
/*****************************************************************************/
void PopupItem::HideAll()
{
    for (int i = 0; i < mItems.size(); i++)
    {
        mItems.at(i)->hide();
    }
    hide();
}
/*****************************************************************************/
void PopupItem::SetSizes(const QRectF &border, const QRectF &card)
{
    mBorder = border;
    mCardSize = card;
}

//=============================================================================
// End of file PopupItem.cpp
//=============================================================================
