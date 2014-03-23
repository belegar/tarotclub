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

#include "MenuItem.h"
#include <QBrush>
#include <QPainter>
#include <QtGlobal>
#include "Common.h"

static const int SPACE = 10;

// Declare here all the buttons managed by the menu
/**
 * @brief text
 *  QString text;
 *  QPointF coord;
 *  MenuWidget widget;
 *  MenuName menu;
 */

static const MenuItem::MenuButton buttonList[] =
{
    { QObject::tr("Pass"),           QPointF(10, 10),                                    MenuItem::PASS_BUTTON, MenuItem::BIDS_MENU },
    { QObject::tr("Take"),           QPointF(TEXT_BOX_WIDTH + 20, 10),                   MenuItem::TAKE_BUTTON, MenuItem::BIDS_MENU  },
    { QObject::tr("Guard"),          QPointF(10, TEXT_BOX_HEIGHT + 20),                  MenuItem::GUARD_BUTTON, MenuItem::BIDS_MENU  },
    { QObject::tr("Guard without"),  QPointF(TEXT_BOX_WIDTH + 20, TEXT_BOX_HEIGHT + 20), MenuItem::GUARD_WITHOUT_BUTTON, MenuItem::BIDS_MENU  },
    { QObject::tr("Guard against"),  QPointF(10, 2 * TEXT_BOX_HEIGHT + 30),              MenuItem::GUARD_AGAINST_BUTTON, MenuItem::BIDS_MENU  },
    { QObject::tr("Handle"),         QPointF(10, 10),                                    MenuItem::DECLARE_HANDLE_BUTTON, MenuItem::HANDLE_MENU  },
    { QObject::tr("Accept"),         QPointF(10, 10),                                    MenuItem::ACCEPT_DISCARD_BUTTON, MenuItem::DISCARD_MENU  },
    { QObject::tr("Start"),          QPointF(10, 10),                                    MenuItem::START_SINGLE_PLAYER, MenuItem::MAIN_MENU  }
};

/*****************************************************************************/
CheckBoxItem::CheckBoxItem(QGraphicsItem *parent)
    :  QGraphicsItem(parent)
    , status(false)
    , tick(":images/tick.svg", this)
    , text(this)
    , square(this)
{
    tick.setPos(0, 0);
    tick.setScale(0.1);
    tick.hide();

    square.setRect(0, 10, 15, 15);
    QPen pen = square.pen();
    pen.setWidth(2);
    square.setPen(pen);

    text.setText("Slam");
    QFont font = text.font();
    font.setBold(true);
    text.setFont(font);
    text.setPos(SPACE + square.boundingRect().width(), 10);
}
/*****************************************************************************/
int CheckBoxItem::type() const
{
    // Enable the use of qgraphicsitem_cast with this item.
    return Type;
}
/*****************************************************************************/
void CheckBoxItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(widget)

}
/*****************************************************************************/
QRectF CheckBoxItem::boundingRect() const
{
    qreal width = square.boundingRect().width() + text.boundingRect().width() + SPACE;
    qreal height = qMax(square.boundingRect().height(), text.boundingRect().height()) + 10;

    QRectF size(0, 0, width, height);
    return size;
}
/*****************************************************************************/
void CheckBoxItem::Click(const QPointF &pos)
{
    if (contains(mapFromParent(pos)))
    {
        if (status)
        {
            status = false;
            tick.hide();
        }
        else
        {
            status = true;
            tick.show();
        }
    }
}
/*****************************************************************************/
MenuItem::MenuItem()
    : color(149, 149, 149, 127)
    , brushSelected(QColor("#404040"))
    , brushNormal(QColor("#808080"))
{
    setRect(0, 0, 260, 130);

    // This menu manages N buttons
    for (int i = 0; i < (sizeof(buttonList)/sizeof(MenuItem::MenuButton)); i++)
    {
        TextBox *tb = new TextBox(buttonList[i].coord);
        tb->setParentItem(this);
        tb->hide();
        tb->SetText(buttonList[i].text);
        tb->setBrush(brushNormal);
        tb->setPen(QPen(Qt::white));

        buttons.insert(&buttonList[i], tb);
    }

    checkBox.setParentItem(this);
    checkBox.setPos(TEXT_BOX_WIDTH + 50, 2 * TEXT_BOX_HEIGHT + 30);
    checkBox.hide();
}
/*****************************************************************************/
int MenuItem::type() const
{
    // Enable the use of qgraphicsitem_cast with this item.
    return Type;
}
/*****************************************************************************/
void MenuItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Paint with specified color and pen
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(QBrush(color));
    painter->setPen(Qt::NoPen);
    painter->drawRoundRect(rect(), (int)(25 * rect().height()
                                         / rect().width()), 25);
}
/*****************************************************************************/
const MenuItem::MenuButton *MenuItem::Refresh(const QPointF &pos, bool clicked)
{
    const MenuItem::MenuButton *button = NULL;

    QMapIterator<const MenuButton *, TextBox *> i(buttons);
    while (i.hasNext())
    {
        i.next();

        if (i.value()->isVisible())
        {
            if (i.value()->contains(mapFromParent(pos)))
            {
                i.value()->setBrush(brushSelected);
                button = i.key();
            }
            else
            {
                i.value()->setBrush(brushNormal);
            }
        }
    }

    // Manage the checkbox
    if (clicked)
    {
        checkBox.Click(mapFromParent(pos));
    }

    return button;
}
/*****************************************************************************/
void MenuItem::DisplayMenu(MenuItem::MenuName menu)
{
    QMapIterator<const MenuButton *, TextBox *> i(buttons);
    while (i.hasNext())
    {
        i.next();
        if (i.key()->menu == menu)
        {
            i.value()->show();
        }
        else
        {
            i.value()->hide();
        }
    }
    checkBox.hide();
}
/*****************************************************************************/
void MenuItem::DisplayMenu(Contract minContract)
{
    QMapIterator<const MenuButton *, TextBox *> i(buttons);
    while (i.hasNext())
    {
        i.next();
        if (i.key()->menu == BIDS_MENU)
        {
            i.value()->show();
            if (i.key()->widget != Contract::PASS)
            {
                if (minContract >= i.key()->widget)
                {
                    i.value()->hide();
                }
            }
        }
        else
        {
            i.value()->hide();
        }
    }
    checkBox.show();
}
/*****************************************************************************/
bool MenuItem::GetSlamOption()
{
    return checkBox.GetStatus();
}

//=============================================================================
// End of file MenuItem.cpp
//=============================================================================
