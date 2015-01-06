/*=============================================================================
 * TarotClub - RectBase.h
 *=============================================================================
 * Base class that manages box rendering and shared style
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

#ifndef RECT_BASE_H
#define RECT_BASE_H

#include <QPen>
#include <QPainter>
#include <QGraphicsRectItem>
#include <QRadialGradient>
#include <QGraphicsDropShadowEffect>

class RectBase : public QGraphicsRectItem
{
public:
    RectBase(bool round)
        : mRound(round)
    {
        mPen.setColor("#C68E17");
        mPen.setWidth(3);

        // Background gradient effect
        mGradient.setColorAt( 0, QColor("#5A310D") );
        mGradient.setColorAt( 1, Qt::black);

        // Create a drop-shadow under the item
        mShadow.setBlurRadius(15.0);
        mShadow.setColor(Qt::black);
        mShadow.setOffset(0, 0);
        setGraphicsEffect(&mShadow);
    }

protected:

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);

        // Paint with specified color and pen
        painter->setRenderHint(QPainter::Antialiasing);

        mGradient.setCenter(boundingRect().width() / 2, boundingRect().height() / 2);
        mGradient.setRadius(boundingRect().width() * 2.0);

        painter->setPen(mPen);
        painter->setBrush(mGradient);
        if (mRound)
        {
            painter->drawRoundRect(rect(), (int)(15 * rect().height() / rect().width()), 15);
        }
        else
        {
            painter->drawRect(rect());
        }
    }

private:
    bool mRound;
    QPen mPen;
    QRadialGradient mGradient;
    QGraphicsDropShadowEffect mShadow;
};

#endif // RECT_BASE_H

//=============================================================================
// End of file RectBase.h
//=============================================================================
