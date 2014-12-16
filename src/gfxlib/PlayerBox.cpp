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
    : RectBase(true)
    , mNameColor("#808080")
    , mName(QPointF(0, 0))
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

    // Init with defaut image
    mAvatar.setParentItem(this);
    mAvatar.setPixmap(QPixmap()); // empty pixmap
    mAvatar.setPos(SPACE_BETWEEN_ITEMS, SPACE_BETWEEN_ITEMS);
    mAvatar.hide();

    // player's name
    mName.setParentItem(this);
    mName.setPen(QPen(QColor(Qt::black), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    mName.show();
    mName.setBrush(mNameColor);

    // bid box
    mBid.setParentItem(this);
    mBid.SetTextColor(Qt::white);
    mBid.show();
    mBid.setBrush(Qt::red);
}
/*****************************************************************************/
void PlayerBox::SetAvatar(const QString &av)
{
    QFile f(av);

    if (f.exists())
    {
        QPixmap img(av);
        mAvatar.setPixmap(img);
    }
    else
    {
        mAvatar.setPixmap(QPixmap()); // empty one
    }
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
        color = Qt::darkRed;
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
        mName.setBrush(Qt::darkYellow);
    }
    else
    {
        mName.setBrush(mNameColor);
    }
    update();
}

//=============================================================================
// End of file PlayerBox.cpp
//=============================================================================
