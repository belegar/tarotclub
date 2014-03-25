/*=============================================================================
 * TarotClub - GfxCard.cpp
 *=============================================================================
 * Graphical tarot card
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

#include "GfxCard.h"

/*****************************************************************************/
void CardShadow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Paint with specified color and pen
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(QBrush(QColor(149, 149, 149, 127)));
    painter->setPen(Qt::NoPen);
    painter->drawRoundRect(rect(), (int)(15 * rect().height()
                                         / rect().width()), 15);
}
/*****************************************************************************/
GfxCard::GfxCard(const QString &fileName, QGraphicsItem *parent) : QGraphicsSvgItem(fileName, parent)

{
    status = NORMAL;
}
/*****************************************************************************/
int GfxCard::type() const
{
    // Enable the use of qgraphicsitem_cast with this item.
    return Type;
}
/*****************************************************************************/
GfxCard::Status GfxCard::GetStatus()
{
    return status;
}
/*****************************************************************************/
void GfxCard::SetStatus(GfxCard::Status s)
{
    status = s;
}
/*****************************************************************************/
void GfxCard::ToggleStatus()
{
    if (status == NORMAL)
    {
        status = SELECTED;
        this->moveBy(0.0, -20.0);
    }
    else
    {
        status = NORMAL;
        this->moveBy(0.0, 20.0);
    }
}

//=============================================================================
// End of file GfxCard.cpp
//=============================================================================
