/*=============================================================================
 * TarotClub - BidsForm.cpp
 *=============================================================================
 * Graphical menu to choose the bids
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

#include "BidsForm.h"
#include <QBrush>
#include <QPainter>

/*****************************************************************************/
BidsForm::BidsForm()
    : color(149, 149, 149, 127)
    , buttonColor(Qt::blue)
    , rect(0, 0, 150, 150)
{
    QBrush brush = QBrush(buttonColor);
    passButton = new QGraphicsRectItem(this);
    passButton->setBrush(brush);
    passButton->setRect(QRectF(0, 0, 60, 40));
}
/*****************************************************************************/
int BidsForm::type() const
{
    // Enable the use of qgraphicsitem_cast with this item.
    return Type;
}
/*****************************************************************************/
void BidsForm::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
   Q_UNUSED(option);
   Q_UNUSED(widget);

   // Paint with specified color and pen
   painter->setRenderHint(QPainter::Antialiasing);
   painter->setBrush(QBrush(color));
   painter->setPen(Qt::NoPen);
   painter->drawRoundRect(rect, 25, 25);
}


//=============================================================================
// End of file BidsForm.cpp
//=============================================================================
