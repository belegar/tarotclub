/*=============================================================================
 * TarotClub - TextBox.h
 *=============================================================================
 * Two text box classes to show names and bids on canvas
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

/*****************************************************************************/
/**
 * @brief Card shadow item to show the positions of cards on the play board
 */
class CardShadow : public QGraphicsRectItem
{

public:
    CardShadow(QRectF &pos, QGraphicsScene *canvas);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

};


/*****************************************************************************/
/**
 * Rectangle box + text inside, for bids and player nicknames
 */
class TextBox : public QGraphicsRectItem
{

private:
    QString  text;
    int penWidth;
    QColor penColor;
    QColor fillColor;

public:
    TextBox(const QPointF &pos, QGraphicsScene *canvas);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void move(int x, int y)
    {
        setPos(x, y);
    }
    void setText(const QString &s)
    {
        text = s;
        text.truncate(10);
    }
    void setPenWidth(int w)
    {
        penWidth = w;
    }
    void setPenColor(Qt::GlobalColor c)
    {
        penColor = c;
    }
    void setFillColor(QColor &c)
    {
        fillColor = c;
    }
};

/*****************************************************************************/
/**
 * Advanced graphics box item with icon
 */
class PlayerBox : public TextBox
{
private:
    QGraphicsPixmapItem      *avatar;

public:
    PlayerBox(const QPointF &pos, QGraphicsScene *canvas);

    void selectPlayer(bool selected);
    void highlightPlayer(bool highlighted);
    void setAvatar(const QString &avatar);
    void enableAvatar(bool enable);

};

#endif // _TEXT_BOX_H

//=============================================================================
// End of file TextBox.h
//=============================================================================
