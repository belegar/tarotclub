/*=============================================================================
 * TarotClub - BidsForm.cpp
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

#include "BidsForm.h"
#include <QBrush>
#include <QPainter>
#include <QtGlobal>
#include "../Tools.h"

static const QPointF coordButtonBox[5] =
{
    QPointF(10, 10),                    // PASS
    QPointF(TEXT_BOX_WIDTH+20, 10),     // TAKE
    QPointF(10, TEXT_BOX_HEIGHT+20),    // GUARD
    QPointF(TEXT_BOX_WIDTH+20, TEXT_BOX_HEIGHT+20), // GUARD WITHOUT
    QPointF(10, 2*TEXT_BOX_HEIGHT+30)   // GUARD AGAINST
};

static const int SPACE = 10;

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
BidsForm::BidsForm()
    : color(149, 149, 149, 127)
    , brushSelected(QColor("#404040"))
    , brushNormal(QColor("#808080"))
{
    setRect(0, 0, 260, 130);

    for (int i=0; i<5; i++)
    {
        TextBox *tb = new TextBox(coordButtonBox[i]);
        tb->setParentItem(this);
        tb->show();
        tb->SetText(Util::ToString((Contract)i));
        tb->setBrush(brushNormal);
        tb->setPen(QPen(Qt::white));

        buttons.insert((Contract)i, tb);
    }

    checkBox.setParentItem(this);
    checkBox.setPos(TEXT_BOX_WIDTH+50, 2*TEXT_BOX_HEIGHT+30);
}
/*****************************************************************************/
int BidsForm::type() const
{
    // Enable the use of qgraphicsitem_cast with this item.
    return Type;
}
/*****************************************************************************/
void BidsForm::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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
bool BidsForm::Refresh(const QPointF &pos, bool clicked, Contract &contract)
{
    contract = PASS;
    bool ret = false;

    QMapIterator<Contract, TextBox *> i(buttons);
    while (i.hasNext())
    {
        i.next();
        if (i.value()->contains(mapFromParent(pos)))
        {
            i.value()->setBrush(brushSelected);
            contract = i.key();
            ret = true;
        }
        else
        {
            i.value()->setBrush(brushNormal);
        }
    }

    // Manage the checkbox
    if (clicked)
    {
        checkBox.Click(mapFromParent(pos));
    }

    return ret;
}
/*****************************************************************************/
void BidsForm::SetMinimalContract(Contract contract)
{
    QMapIterator<Contract, TextBox *> i(buttons);
    while (i.hasNext())
    {
        i.next();
        i.value()->show();
        if (i.key() != PASS)
        {
            if (contract >= i.key())
            {
                i.value()->hide();
            }
        }
    }
}

//=============================================================================
// End of file BidsForm.cpp
//=============================================================================
