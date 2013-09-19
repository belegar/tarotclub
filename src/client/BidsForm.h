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
#include "TextBox.h"
#include <QtGui>
#include "../defines.h"

/*****************************************************************************/
class BidsForm : public QGraphicsRectItem
{
public:
    BidsForm();

    enum { Type = UserType + 2 };
    int type() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    bool Refresh(const QPointF &pos, Contract &contract);
    void SetMinimalContract(Contract contract);

private:
    QColor color;

    QMap<Contract, TextBox *> buttons;
};

#endif // BIDSFORM_H

//=============================================================================
// End of file BidsForm.h
//=============================================================================
