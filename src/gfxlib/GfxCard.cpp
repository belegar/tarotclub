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

static const qreal SCALE_FACTOR = 1.5f;

/*****************************************************************************/
GfxCard::GfxCard(const QString &fileName, ICardEvent *event, const Card &card, QGraphicsItem *parent)
    : QGraphicsSvgItem(fileName, parent)
    , mSelected(false)
    , mEvent(event)
    , mCard(card)
{
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);

    setScale(SCALE_FACTOR);

    // Create a drop-shadow under the card
    mShadow.setBlurRadius(15.0);
    mShadow.setColor(Qt::black);
    mShadow.setOffset(0, 0);
    setGraphicsEffect(&mShadow);

    // Create a radial gradient to make the cards old-style
    QRectF size = GetRealSize();

    mGradient.setCenter(size.width() / 2, size.height() / 2);
    mGradient.setRadius(size.width() * 1.5);
    mGradient.setColorAt( 0, Qt::white );
    mGradient.setColorAt( 1, QColor(244, 238, 215));
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
QRectF GfxCard::GetRealSize()
{
    QRectF cardSize = boundingRect();
    cardSize.setWidth(cardSize.width() * SCALE_FACTOR);
    cardSize.setHeight(cardSize.height() * SCALE_FACTOR);
    return cardSize;
}
/*****************************************************************************/
bool GfxCard::IsEqual(const Card &c)
{
    return (c == mCard);
}
/*****************************************************************************/
void GfxCard::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    (void)event;
    if (mEvent != NULL)
    {
        mEvent->CardHoverEnter(mCard.GetValue(), mCard.GetSuit());
    }
}
/*****************************************************************************/
void GfxCard::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    (void)event;
    if (mEvent != NULL)
    {
        mEvent->CardHoverLeave(mCard.GetValue(), mCard.GetSuit());
    }
}
/*****************************************************************************/
void GfxCard::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    (void)event;
    if (mEvent != NULL)
    {
        mEvent->CardClicked(mCard.GetValue(), mCard.GetSuit(), mSelected);
    }
}

//=============================================================================
// End of file GfxCard.cpp
//=============================================================================
