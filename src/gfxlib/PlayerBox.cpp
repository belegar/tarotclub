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

#define SPACE_BETWEEN_ITEMS 10
#define PLAYER_BOX_WIDTH    240
#define PLAYER_BOX_HEIGHT   (TEXT_BOX_WIDTH+2*TEXT_BOX_HEIGHT+4*SPACE_BETWEEN_ITEMS)

/*****************************************************************************/
PlayerBox::PlayerBox(const QRectF &cardSize)
    : name(QPointF(0, 0))
    , bid(QPointF(0, 0))
    , mCardSize(cardSize)
{
    setRect(0, 0, PLAYER_BOX_WIDTH, PLAYER_BOX_HEIGHT);

    // Init with defaut image
    avatar.setParentItem(this);
    avatar.setPixmap(QPixmap(":/images/vide.png"));
    avatar.setPos(SPACE_BETWEEN_ITEMS, SPACE_BETWEEN_ITEMS);
    avatar.hide();

    // player's name is under the avatar
    name.setParentItem(this);
    name.setPen(QPen(QColor(Qt::black), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    name.setPos(SPACE_BETWEEN_ITEMS, avatar.boundingRect().height() + 2 * SPACE_BETWEEN_ITEMS);
    name.show();
    SetBackgroundColor(Qt::gray, NAME_BOX);

    // bid box is under the player's name
    bid.setParentItem(this);
    bid.setPos(SPACE_BETWEEN_ITEMS, avatar.boundingRect().height() + name.boundingRect().height() + 3 * SPACE_BETWEEN_ITEMS);
    bid.SetTextColor(Qt::white);
    bid.show();
    SetBackgroundColor(Qt::red, BID_BOX);

    // The card shadow is located on the right side of the player box
    mCardShadow.setParentItem(this);
    mCardShadow.setRect(mCardSize);

    mCardShadow.setPos(name.boundingRect().width() + 2 * SPACE_BETWEEN_ITEMS, (PLAYER_BOX_HEIGHT - mCardShadow.boundingRect().height()) / 2);
    mCardShadow.show();
    mCardShadow.setZValue(0);
}
/*****************************************************************************/
void PlayerBox::SetBackgroundColor(Qt::GlobalColor color, Box box)
{
    QPointF start, stop;

    if (box == NAME_BOX)
    {
        start = name.rect().topLeft();
        stop = name.rect().bottomLeft();
    }
    else
    {
        start = bid.rect().topLeft();
        stop = bid.rect().bottomLeft();
    }

    QLinearGradient gradient(start, stop);

    gradient.setColorAt(0.0, Qt::transparent);
    gradient.setColorAt(1.0, color);

    if (box == NAME_BOX)
    {
        name.setBrush(gradient);
    }
    else
    {
        bid.setBrush(gradient);
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

    avatar.setPixmap(img);
}
/*****************************************************************************/
void PlayerBox::SetBidText(const QString &text)
{
    bid.SetText(text);
    update();
}
/*****************************************************************************/
void PlayerBox::SetPlayerName(const QString &text)
{
    name.SetText(text);
    update();
}
/*****************************************************************************/
void PlayerBox::EnableAvatar(bool enable)
{
    if (enable == true)
    {
        avatar.show();
    }
    else
    {
        avatar.hide();
    }
}
/*****************************************************************************/
void PlayerBox::DrawCard(GfxCard *c)
{
    if (c)
    {
        c->setPos(mapToScene(mCardShadow.pos()));
        c->setZValue(1);
        c->show();
    }
    else
    {
        TLogError("Card is null, cannot display it!");
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
    name.setPen(QPen(QColor(color), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    name.update();
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
