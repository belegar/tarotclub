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
#include <QtGui>
#include "TextBox.h"
#include "Common.h"
#include "CustomTypes.h"

/*****************************************************************************/
class CheckBoxItem : public QGraphicsItem
{
public:
    CheckBoxItem(QGraphicsItem *parent = 0);

    enum { Type = UserType + CHECK_BOX_TYPE_ITEM };
    int type() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect() const;

    void Click(const QPointF &pos);
    bool GetStatus()
    {
        return status;
    }

private:
    bool status;
    QGraphicsSvgItem tick;
    QGraphicsSimpleTextItem text;
    QGraphicsRectItem square;
};

/*****************************************************************************/
class MenuItem : public QGraphicsRectItem
{
public:
    MenuItem();

    /**
     * @brief The MenuWidget enum gather all the widgets managed by this menu
     *
     * Just add a new enum line to manage this widget
     */
    enum MenuWidget
    {
        NO_ENTRY = -1,   //!< Only required entry, do not delete

        // Add the widgets below
        PASS_BUTTON = 0,
        TAKE_BUTTON = 1,
        GUARD_BUTTON = 2,
        GUARD_WITHOUT_BUTTON = 3,
        GUARD_AGAINST_BUTTON = 4,
        DECLARE_HANDLE_BUTTON = 5,
        ACCEPT_DISCARD_BUTTON = 6,
        START_SINGLE_PLAYER = 7
    };

    enum MenuName
    {
        NO_MENU,
        BIDS_MENU,
        HANDLE_MENU,
        DISCARD_MENU,
        MAIN_MENU
    };

    struct MenuButton
    {
        QString text;
        QPointF coord;
        int widget;
        MenuName menu;
    };

    enum { Type = UserType + MENU_TYPE_ITEM };

    // Virtual methods
    int type() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    // Helpers
    const MenuButton *Refresh(const QPointF &pos, bool clicked);
    void DisplayMenu(MenuName menu);
    void DisplayMenu(Contract minContract);

    // Getters
    bool GetSlamOption();

private:
    QColor color;
    QBrush brushSelected;
    QBrush brushNormal;

    QMap<const MenuButton *, TextBox *> buttons;
    CheckBoxItem checkBox;
};

#endif // BIDSFORM_H

//=============================================================================
// End of file BidsForm.h
//=============================================================================
