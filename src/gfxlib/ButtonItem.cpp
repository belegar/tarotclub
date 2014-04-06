/*=============================================================================
 * TarotClub - ButtonItem.cpp
 *=============================================================================
 * Graphical clickable button with mouse over highlight
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

#include "ButtonItem.h"

ButtonItem::ButtonItem(IButtonEvent &event, std::uint8_t id, std::uint8_t menu)
    : TextBox(QPointF())
    , mBrushSelected(QColor("#404040"))
    , mBrushNormal(QColor("#808080"))
    , mEvent(&event)
    , mId(id)
    , mMenu(menu)
{
    setBrush(mBrushNormal);
    setPen(QPen(Qt::white));
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);
}

void ButtonItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    (void)event;
    setBrush(mBrushSelected);
}

void ButtonItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    (void)event;
    setBrush(mBrushNormal);
}

void ButtonItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    (void)event;
    if (mEvent != NULL)
    {
        mEvent->Clicked(mId, mMenu);
    }
}

void ButtonItem::HighLight(bool state)
{
    (void)state;
}

//=============================================================================
// End of file ButtonItem.cpp
//=============================================================================
