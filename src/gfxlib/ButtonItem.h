/*=============================================================================
 * TarotClub - ButtonItem.h
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

#ifndef BUTTON_ITEM_H_
#define BUTTON_ITEM_H_

#include "TextBox.h"
#include "IButtonEvent.h"

class ButtonItem : public TextBox
{
public:
    ButtonItem(IButtonEvent &event, std::uint8_t id, std::uint8_t menu);

    void HighLight(bool state);

    std::uint32_t GetId() { return mId; }

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    QBrush mBrushSelected;
    QBrush mBrushNormal;
    IButtonEvent *mEvent;
    std::uint8_t mId;
    std::uint8_t mMenu;
};

#endif // BUTTON_ITEM_H_

//=============================================================================
// End of file ButtonItem.h
//=============================================================================
