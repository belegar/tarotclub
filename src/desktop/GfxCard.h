/*=============================================================================
 * TarotClub - GfxCard.h
 *=============================================================================
 * Graphical tarot card
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

#ifndef GFXCARD_H
#define GFXCARD_H

#include <QtSvg>
#include "CustomTypes.h"

/*****************************************************************************/
/**
 * @brief Card shadow item to show the positions of cards on the play board
 */
class CardShadow : public QGraphicsRectItem
{

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
};
/*****************************************************************************/
class GfxCard : public QGraphicsSvgItem
{
public:
    enum Status
    {
        NORMAL,
        SELECTED
    };

    GfxCard(const QString &fileName, QGraphicsItem *parent = 0);

    enum { Type = UserType + CARD_TYPE_ITEM };
    int type() const;
    Status GetStatus();
    void SetStatus(Status s);
    void ToggleStatus();

private:
    Status status;
};

#endif // GFXCARD_H

//=============================================================================
// End of file GfxCard.h
//=============================================================================
