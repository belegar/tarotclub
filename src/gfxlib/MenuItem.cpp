/*=============================================================================
 * TarotClub - MenuItem.cpp
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

// Qt includes
#include <QBrush>
#include <QPainter>
#include <QtGlobal>
#include <QApplication>

// Game includes
#include "Common.h"
#include "MenuItem.h"
#include "PlayerBox.h"

static const std::uint32_t SPACE = 10;

// Declare here all the buttons managed by the menu
/**
 * @brief text
 *  QString text;
 *  QPointF coord;
 *  MenuWidget widget;
 *  MenuName menu;
 */

static const std::uint8_t buttonMenu[] =
{
    MenuItem::BIDS_MENU,
    MenuItem::BIDS_MENU,
    MenuItem::BIDS_MENU,
    MenuItem::BIDS_MENU,
    MenuItem::BIDS_MENU,
    MenuItem::HANDLE_MENU,
    MenuItem::DISCARD_MENU,
    MenuItem::MAIN_MENU
};

/*****************************************************************************/
MenuItem::MenuItem(IButtonEvent *event)
    : color(149, 149, 149, 127)
    , checkBox(this, SPACE)
{
    setRect(0, 0, PlayerBox::PLAYER_BOX_WIDTH_HOR, 130);

#ifdef TAROT_DEBUG
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
#endif

    // This menu manages N buttons
    for (std::uint8_t i = 0U; i < sizeof(buttonMenu); i++)
    {
        ButtonItem *button = new ButtonItem(event, i, buttonMenu[i]);
        button->setParentItem(this);
        button->hide();
        buttons.push_back(button);
    }

    checkBox.hide();
}
/*****************************************************************************/
int MenuItem::type() const
{
    // Enable the use of qgraphicsitem_cast with this item.
    return Type;
}
/*****************************************************************************/
void MenuItem::Initialize()
{
    QString labels[] =
    {
        QObject::tr(Contract::STR_PASS.c_str()),
        QObject::tr(Contract::STR_TAKE.c_str()),
        QObject::tr(Contract::STR_GUARD.c_str()),
        QObject::tr(Contract::STR_GUARD_WITHOUT.c_str()),
        QObject::tr(Contract::STR_GUARD_AGAINST.c_str()),
        QObject::tr("Handle"),
        QObject::tr("Accept"),
        QObject::tr("Quick start")
    };

    for (std::uint8_t i = 0U; i < sizeof(buttonMenu); i++)
    {
        buttons[i]->SetText(labels[i]);
    }
    checkBox.SetText(QObject::tr("Slam"));
}
/*****************************************************************************/
void MenuItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Paint with specified color and pen
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(QBrush(color));
    painter->setPen(QPen(Qt::white));
    painter->drawRoundRect(rect(), (int)(25 * rect().height()
                                         / rect().width()), 25);
 }
/*****************************************************************************/
void MenuItem::DisplayMenu(std::uint8_t menu)
{
    std::vector<ButtonItem *>::iterator iter;
    std::uint32_t pos = 0U;

    for (iter = buttons.begin(); iter != buttons.end(); ++iter)
    {
        if ((*iter)->GetMenu() == menu)
        {
            (*iter)->setPos(GetButtonPosition(pos));
            (*iter)->show();
            (*iter)->HighLight(false);
            pos++;
        }
        else
        {
            (*iter)->hide();
        }
    }
    checkBox.hide();
}
/*****************************************************************************/
void MenuItem::DisplayMenu(Contract minContract)
{
    std::vector<ButtonItem *>::iterator iter;
    std::uint32_t pos = 0U;

    for (iter = buttons.begin(); iter != buttons.end(); ++iter)
    {
        if ((*iter)->GetMenu() == BIDS_MENU)
        {
            (*iter)->setPos(GetButtonPosition(pos));
            (*iter)->show();
            (*iter)->HighLight(false);
            std::uint32_t id = (*iter)->GetId();
            if (id != PASS_BUTTON)
            {
                if (minContract >= Contract(id))
                {
                    (*iter)->hide();
                }
            }
            pos++;
        }
        else
        {
            (*iter)->hide();
        }
    }
    checkBox.setPos(GetButtonPosition(pos));
    checkBox.show();
}
/*****************************************************************************/
/**
 * @brief MenuItem::MoveButton
 *
 * Example: box size is 100 (width) x 30 (hight), SPACE pixels betwen buttons
 *
 * line   |        colum 1                 |              column 2
 * -------|--------------------------------|------------------------------------------
 *    0   |   0 (10, 10)                   |   1 (10 + 100 + SPACE, 10)
 *    1   |   2 (10, 10 + 30 + SPACE)      |   3 (10 + 100 + SPACE, 10 + 30 + SPACE)
 *    2   |   4 (10, 10 + 2 *(30 + SPACE)) |   5 (10 + 100 + SPACE, 10 + 2 *(30 + SPACE))
 *
 * @param pos, from 0, top left position
 */
QPointF MenuItem::GetButtonPosition(std::uint32_t pos)
{
    QPointF coord;
    // Start at the upper-left corner
    std::uint32_t line = (pos / 2);
    coord.setY(10U + line * (TEXT_BOX_HEIGHT + SPACE));
    std::uint32_t x = 10U;

    // odd position: move x coordinate
    if (pos & 1)
    {
        // Second column
        x = x + TEXT_BOX_WIDTH + SPACE;
    }

    coord.setX(x);
    return coord;
}
/*****************************************************************************/
bool MenuItem::GetSlamOption()
{
    return checkBox.GetStatus();
}
/*****************************************************************************/
void MenuItem::ClearSlamOption()
{
    checkBox.Clear();
}

//=============================================================================
// End of file MenuItem.cpp
//=============================================================================
