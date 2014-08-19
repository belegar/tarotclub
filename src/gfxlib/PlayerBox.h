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

#ifndef PLAYER_BOX_H
#define PLAYER_BOX_H

// Qt includes
#include "TextBox.h"
#include "GfxCard.h"

/*****************************************************************************/
class AvatarItem: public QGraphicsPixmapItem
{
public:
    AvatarItem()
        : mSize(TEXT_BOX_WIDTH)
    {
        setCacheMode(NoCache);
    }
    ~AvatarItem() {}

public:

    QRectF boundingRect() const
    {
        // Return defined 'size'
        return QRectF(QPointF(0, 0), QSizeF(mSize, mSize));
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
    {
        // Scale QGraphicsPixmapItem to wanted 'size' and keep the aspect ratio using boundingRect()
        painter->drawPixmap(boundingRect().toRect(), pixmap());
        Q_UNUSED(option);
        Q_UNUSED(widget);
    }

private:
    int mSize;
};

/*****************************************************************************/
/**
 * @brief A PlayerBox is a parent item composed by:
 *   - an avatar image
 *   - a nickname text box
 *   - a bid text box
 */
class PlayerBox : public QGraphicsRectItem
{

public:
    PlayerBox(const QRectF &cardSize);

    // Helpers
    void SelectPlayer(bool selected);
    void HighlightPlayer(bool highlighted);
    void EnableAvatar(bool enable);
    void DrawCard(GfxCard *c);

    // Setters
    void SetAvatar(const QString &avatar);
    void SetBidText(const QString &text);
    void SetPlayerName(const QString &text);

private:
    enum Box
    {
        NAME_BOX,
        BID_BOX
    };

    void SetBackgroundColor(Qt::GlobalColor color, Box box);
    AvatarItem avatar;
    TextBox name;
    TextBox bid;
    CardShadow mCardShadow;
    QRectF mCardSize;
};

#endif // PLAYER_BOX_H

//=============================================================================
// End of file PlayerBox.h
//=============================================================================
