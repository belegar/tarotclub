/*=============================================================================
 * TarotClub - PlayerBox.h
 *=============================================================================
 * Graphical area to show a player information (avatar, card, name, bids ...)
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

#include "PlayerBox.h"
#include <QFile>
#include <QtGui>

/*****************************************************************************/
PlayerBox::PlayerBox(const QPointF &pos)
    : TextBox(pos)
{
 //   QColor color(255, 255, 255, 127); // transparent

    setPen(QPen(QColor(Qt::black), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    // Init with defaut image
    avatar = new AvatarItem(QPixmap(":/images/vide.png"));
    avatar->setSize(40);
    avatar->hide();

    // we set it to the right place
    avatar->setPos(rect().x() + rect().width() + 10, rect().y() - 5);
}
/*****************************************************************************/
void PlayerBox::SetBackgroundColor(Qt::GlobalColor color)
{
    QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());

    gradient.setColorAt(0.0, Qt::transparent);
    gradient.setColorAt(1.0, color);
    setBrush(gradient);
}
/*****************************************************************************/
void PlayerBox::SetAvatar(const QString &av)
{
    QFile f(av);

    if (f.exists() == false)
    {
        return;
    }
    QPixmap img(av);
  //  img.scaledToHeight(40);

    avatar->setPixmap(img);
}
/*****************************************************************************/
void PlayerBox::EnableAvatar(bool enable)
{
    if (enable == true)
    {
        avatar->show();
    }
    else
    {
        avatar->hide();
    }
}
/*****************************************************************************/
void PlayerBox::SelectPlayer(bool selected)
{
    Qt::GlobalColor color;
    qreal width;

    if (selected == true)
    {
        width = 2;
        color = Qt::red;
    }
    else
    {
        width = 1;
        color = Qt::black;
    }
    setPen(QPen(QColor(color), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    update();
}
/*****************************************************************************/
void PlayerBox::highlightPlayer(bool highlighted)
{
    Qt::GlobalColor color;

    if (highlighted == true)
    {
        color = Qt::yellow;
    }
    else
    {
        color = Qt::transparent;
    }

    setPen(QPen(QColor(color), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    update();
}

//=============================================================================
// End of file PlayerBox.cpp
//=============================================================================
