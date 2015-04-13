/*=============================================================================
 * TarotClub - CheckBoxItem.h
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

#ifndef CHECKBOXITEM_H
#define CHECKBOXITEM_H

// LibC++ includes
#include <cstdint>

// Qt includes
#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>
#include <QtGui>

// Game includes
#include "CustomTypes.h"

/*****************************************************************************/
class CheckBoxItem : public QGraphicsItem
{
public:
    CheckBoxItem(QGraphicsItem *parent = 0, std::uint32_t space = 10U);

    enum { Type = UserType + CHECK_BOX_TYPE_ITEM };
    int type() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect() const;

    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    bool GetStatus()
    {
        return status;
    }

    void Clear()
    {
        status = false;
        tick.hide();
    }

    void SetText(const QString &checkText)
    {
        text.setText(checkText);
    }

private:
    bool status;
    QGraphicsSvgItem tick;
    QGraphicsSimpleTextItem text;
    QGraphicsRectItem square;
    std::uint32_t mSpace;
};

#endif // CHECKBOXITEM_H

//=============================================================================
// End of file CheckBoxItem.h
//=============================================================================
