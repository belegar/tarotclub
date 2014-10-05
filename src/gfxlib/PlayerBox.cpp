/*=============================================================================
 * TarotClub - PlayerBox.h
 *=============================================================================
 * Graphical area to show a player information (avatar, card, name, bids ...)
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

#include "PlayerBox.h"
#include <QFile>
#include <QtGui>
#include "Log.h"


/*****************************************************************************/
PlayerBox::PlayerBox(Layout layout)
    : mName(QPointF(0, 0))
    , mBid(QPointF(0, 0))
    , mLayout(layout)
{
#ifdef TAROT_DEBUG
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
#endif

    if (mLayout == VERTICAL)
    {
        setRect(0, 0, PLAYER_BOX_WIDTH_VERT, PLAYER_BOX_HEIGHT_VERT);
        mName.setPos(SPACE_BETWEEN_ITEMS, mAvatar.boundingRect().height() + 2 * SPACE_BETWEEN_ITEMS);
        mBid.setPos(SPACE_BETWEEN_ITEMS, mAvatar.boundingRect().height() + mName.boundingRect().height() + 3 * SPACE_BETWEEN_ITEMS);
    }
    else
    {
        setRect(0, 0, PLAYER_BOX_WIDTH_HOR, PLAYER_BOX_HEIGHT_HOR);
        mName.setPos(2 * SPACE_BETWEEN_ITEMS + mAvatar.boundingRect().width(), SPACE_BETWEEN_ITEMS);
        mBid.setPos(2 * SPACE_BETWEEN_ITEMS + mAvatar.boundingRect().width(), mName.boundingRect().height() + 2 * SPACE_BETWEEN_ITEMS);
    }

    QRadialGradient gradient;
    gradient.setCenter(boundingRect().width() / 2, boundingRect().height() / 2);
    gradient.setRadius(boundingRect().width() * 2.0);
    gradient.setColorAt( 0, QColor("#5A310D") );
    gradient.setColorAt( 1, Qt::black);
    setBrush(gradient);

    // Init with defaut image
    mAvatar.setParentItem(this);
    mAvatar.setPixmap(QPixmap(":/images/vide.png"));
    mAvatar.setPos(SPACE_BETWEEN_ITEMS, SPACE_BETWEEN_ITEMS);
    mAvatar.hide();

    // player's name
    mName.setParentItem(this);
    mName.setPen(QPen(QColor(Qt::black), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    mName.show();
    SetBackgroundColor(Qt::gray, NAME_BOX);

    // bid box
    mBid.setParentItem(this);
    mBid.SetTextColor(Qt::white);
    mBid.show();
    SetBackgroundColor(Qt::red, BID_BOX);

    // Create a drop-shadow under the card
    mShadow.setBlurRadius(15.0);
    mShadow.setColor(Qt::black);
    mShadow.setOffset(0, 0);
    setGraphicsEffect(&mShadow);
}
/*****************************************************************************/
void PlayerBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Paint with specified color and pen
    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen;
    pen.setColor("#C68E17");
    pen.setWidth(3);
    painter->setPen(pen);
    painter->setBrush(brush());
    painter->drawRoundRect(rect(), (int)(15 * rect().height()
                                         / rect().width()), 15);
}
/*****************************************************************************/
void PlayerBox::SetBackgroundColor(Qt::GlobalColor color, Box box)
{
    QPointF start, stop;

    if (box == NAME_BOX)
    {
        start = mName.rect().topLeft();
        stop = mName.rect().bottomLeft();
    }
    else
    {
        start = mBid.rect().topLeft();
        stop = mBid.rect().bottomLeft();
    }

    QLinearGradient gradient(start, stop);

    gradient.setColorAt(0.0, Qt::transparent);
    gradient.setColorAt(1.0, color);

    if (box == NAME_BOX)
    {
        mName.setBrush(gradient);
    }
    else
    {
        mBid.setBrush(gradient);
    }
}
/*****************************************************************************/
void PlayerBox::SetAvatar(const QString &av)
{
    QFile f(av);

    if (f.exists() == false)
    {
        return;
    }
    QPixmap img(av);
    //  img.scaledToHeight(40);

    mAvatar.setPixmap(img);
}
/*****************************************************************************/
void PlayerBox::SetBidText(const QString &text)
{
    mBid.SetText(text);
    update();
}
/*****************************************************************************/
void PlayerBox::SetPlayerName(const QString &text)
{
    mName.SetText(text);
    update();
}
/*****************************************************************************/
void PlayerBox::EnableAvatar(bool enable)
{
    if (enable == true)
    {
        mAvatar.show();
    }
    else
    {
        mAvatar.hide();
    }
}
/*****************************************************************************/
void PlayerBox::SelectPlayer(bool selected)
{
    Qt::GlobalColor color;
    qreal width;

    if (selected == true)
    {
        width = 2;
        color = Qt::red;
    }
    else
    {
        width = 1;
        color = Qt::black;
    }
    mName.setPen(QPen(QColor(color), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    mName.update();
}
/*****************************************************************************/
void PlayerBox::HighlightPlayer(bool highlighted)
{
    if (highlighted == true)
    {
        SetBackgroundColor(Qt::yellow, NAME_BOX);
    }
    else
    {
        SetBackgroundColor(Qt::gray, NAME_BOX);
    }
    update();
}

//=============================================================================
// End of file PlayerBox.cpp
//=============================================================================
