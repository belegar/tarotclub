/*=============================================================================
 * TarotClub - BidsForm.h
 *=============================================================================
 * Graphical menu to choose the bids
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

#ifndef BIDSFORM_H
#define BIDSFORM_H

#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>
#include "TextBox.h"
#include <QtGui>
#include "../defines.h"


/*****************************************************************************/
class CheckBoxItem : public QGraphicsItem
{
public:
    CheckBoxItem(QGraphicsItem *parent = 0);

    enum { Type = UserType + 3 };
    int type() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect() const;

    void Click(const QPointF &pos);
    bool GetStatus() { return status; }

private:
    bool status;
    QGraphicsSvgItem tick;
    QGraphicsSimpleTextItem text;
    QGraphicsRectItem square;
};

/*****************************************************************************/
class BidsForm : public QGraphicsRectItem
{
public:
    BidsForm();

    enum { Type = UserType + 2 };

    // Virtual methods
    int type() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    // Helpers
    bool Refresh(const QPointF &pos, bool clicked, Contract &contract);

    // Setters
    void SetMinimalContract(Contract contract);

    // Getters
    bool GetSlamOption();

private:
    QColor color;
    QBrush brushSelected;
    QBrush brushNormal;

    QMap<Contract, TextBox *> buttons;
    CheckBoxItem checkBox;
};

#endif // BIDSFORM_H

//=============================================================================
// End of file BidsForm.h
//=============================================================================
