/*=============================================================================
 * TarotClub - TextBox.h
 *=============================================================================
 * Generic graphical item that includes a rounded rectangle with a text inside
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

#ifndef _TEXT_BOX_H
#define _TEXT_BOX_H

// Qt includes
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsScene>
#include <QPainter>

#define TEXT_BOX_WIDTH  115
#define TEXT_BOX_HEIGHT 30


/*****************************************************************************/
class TextBox : public QGraphicsRectItem
{

public:
    TextBox(const QPointF &pos);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void SetText(const QString &s)
    {
        text = s;
        text.truncate(15);
    }

    void SetFont(const QFont &f)
    {
        font = f;
    }

    void SetTextColor(Qt::GlobalColor c)
    {
        mTextColor = c;
    }

private:
    QString text;
    QFont font;
    Qt::GlobalColor mTextColor;
};


#endif // _TEXT_BOX_H

//=============================================================================
// End of file TextBox.h
//=============================================================================
