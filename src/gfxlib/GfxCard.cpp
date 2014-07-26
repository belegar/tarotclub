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
GfxCard::GfxCard(const QString &fileName, ICardEvent *event, std::uint8_t id, QGraphicsItem *parent)
    : QGraphicsSvgItem(fileName, parent)
    , mSelected(false)
    , mEvent(event)
    , mId(id)
{
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);


    shadow.setBlurRadius(15.0);
    shadow.setColor(Qt::black);
    shadow.setOffset(0,0);
    setGraphicsEffect(&shadow);
}
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
int GfxCard::type() const
{
    // Enable the use of qgraphicsitem_cast with this item.
    return Type;
}
/*****************************************************************************/
bool GfxCard::IsSelected()
{
    return mSelected;
}
/*****************************************************************************/
void GfxCard::SetSelected(bool s)
{
    mSelected = s;
}
/*****************************************************************************/
void GfxCard::ToggleSelection()
{
    if (!mSelected)
    {
        mSelected = true;
        this->moveBy(0.0, -20.0);
    }
    else
    {
        mSelected = false;
        this->moveBy(0.0, 20.0);
    }
}
/*****************************************************************************/
void GfxCard::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    (void)event;
    if (mEvent != NULL)
    {
        mEvent->CardHoverEnter(mId);
    }
}
/*****************************************************************************/
void GfxCard::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    (void)event;
    if (mEvent != NULL)
    {
        mEvent->CardHoverLeave(mId);
    }
}
/*****************************************************************************/
void GfxCard::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    (void)event;
    if (mEvent != NULL)
    {
        mEvent->CardClicked(mId, mSelected);
    }
}

//=============================================================================
// End of file GfxCard.cpp
//=============================================================================
