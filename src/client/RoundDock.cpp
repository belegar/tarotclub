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

    for (i = 0; i < 4; i++)
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
void RoundDock::Clear()
{
    int i, j;
    QBrush brush(Qt::white);

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 18; j++)
        {
            ui.tableWidget->item(j, i)->setText("");
            ui.tableWidget->item(j, i)->setBackground(brush);
        }
    }
}
/*****************************************************************************/
void RoundDock::AddRound(Game &info, Place p, QString txt)
{
    ui.tableWidget->item(info.trickCounter, p)->setText(txt);
}
/*****************************************************************************/
void RoundDock::SelectWinner(Game &info, Place p)
{
    QBrush brush(Qt::darkGreen);
    ui.tableWidget->item(info.trickCounter, p)->setBackground(brush);
}
/*****************************************************************************/
void RoundDock::SelectFirstPlayer(int turn, Place p)
{
    QBrush brush(Qt::lightGray);
    ui.tableWidget->item(turn, p)->setBackground(brush);
}

//=============================================================================
// Fin du fichier RoundDock.cpp
//=============================================================================
