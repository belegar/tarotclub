/*=============================================================================
 * TarotClub - RoundDock.cpp
 *=============================================================================
 * Fenêtre dockable affichant les plis réalisés à chaque tour de jeu
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

#include <QHeaderView>
#include "RoundDock.h"

/*****************************************************************************/
RoundDock::RoundDock(QWidget *parent)
    : QDockWidget(parent)
{
    int i, j;
    ui.setupUi(this);

    ui.tableWidget->setRowCount(18);

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 18; j++)
        {
            ui.tableWidget->setItem(j, i, new QTableWidgetItem());
        }
    }
}
/*****************************************************************************/
void RoundDock::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);
    emit sgnlClose();
}
/*****************************************************************************/
void RoundDock::clear()
{
    int i, j;
    QBrush brush(Qt::white);

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 18; j++)
        {
            ui.tableWidget->item(j, i)->setText("");
            ui.tableWidget->item(j, i)->setBackground(brush);
        }
    }
}
/*****************************************************************************/
void RoundDock::addRound(int turn, Place p, QString txt)
{
    ui.tableWidget->item(turn, p + 1)->setText(txt);
}
/*****************************************************************************/
void RoundDock::selectWinner(Game &info, Place p)
{
    QBrush brush(Qt::darkGreen);
    ui.tableWidget->item(info.trickCounter, p + 1)->setBackground(brush);
}
/*****************************************************************************/
void RoundDock::pointsToTaker(int turn, float points)
{
    QString txt;

    txt.setNum(points);
    ui.tableWidget->item(turn, 0)->setText(txt);
}
/*****************************************************************************/
void RoundDock::selectFirstPlayer(int turn, Place p)
{
    QBrush brush(Qt::lightGray);
    ui.tableWidget->item(turn, p + 1)->setBackground(brush);
}

//=============================================================================
// Fin du fichier RoundDock.cpp
//=============================================================================
