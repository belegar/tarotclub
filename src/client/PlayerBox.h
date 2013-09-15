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

#ifndef _PLAYER_BOX_H
#define _PLAYER_BOX_H

// Qt includes
#include "TextBox.h"

/*****************************************************************************/
/**
 * Advanced graphics box item with icon
 */
class PlayerBox : public TextBox
{
private:

    class AvatarItem: public QGraphicsPixmapItem
    {
    public:
        AvatarItem(const QPixmap &pixmap, QGraphicsItem *parentItem=0, int size=0)
            : QGraphicsPixmapItem(pixmap,parentItem)
        {
            setCacheMode(NoCache);
            setSize(size);
        }
        ~AvatarItem() {}

    public:
        QRectF boundingRect() const
        {
            // Return defined 'size'
            return QRectF(QPointF(0,0),QSizeF(mSize,mSize));
        }
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget=0)
        {
            // Scale QGraphicsPixmapItem to wanted 'size' and keep the aspect ratio using boundingRect()
            painter->drawPixmap(boundingRect().toRect(), pixmap());
            Q_UNUSED(option);
            Q_UNUSED(widget);

            // NOTE: Does not use base class paint for painting now, that does not scale QPixmap
            //QGraphicsPixmapItem::paint(painter, option, widget);
        }
        void setSize(int size)
        {
            mSize = size;
        }

    private:
        int mSize;
    };


    void SetBackgroundColor(Qt::GlobalColor color);
    AvatarItem *avatar;


public:
    PlayerBox(const QPointF &pos);

    void SelectPlayer(bool selected);
    void HighlightPlayer(bool highlighted);
    void SetAvatar(const QString &avatar);
    void EnableAvatar(bool enable);

};

#endif // _PLAYER_BOX_H

//=============================================================================
// End of file PlayerBox.h
//=============================================================================
