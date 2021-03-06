/*=============================================================================
 * TarotClub - ScoresDock.cpp
 *=============================================================================
 * Fenêtre dockable affichant les scores des joueurs
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
#include "ScoresDock.h"

/*****************************************************************************/
ScoresDock::ScoresDock(QWidget *parent)
    : QDockWidget(tr("Scores"), parent)
{
    tableScores = new QTableWidget(this);
    tableScores->setSelectionMode(QAbstractItemView::NoSelection);
    tableScores->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableScores->setDragEnabled(false);
    tableScores->setSelectionMode(QAbstractItemView::SingleSelection);
    tableScores->setSelectionBehavior(QAbstractItemView::SelectRows);

    QHeaderView *hv = tableScores->horizontalHeader();
    hv->setSectionResizeMode(QHeaderView::Stretch);
    setWidget(tableScores);
}
/*****************************************************************************/
void ScoresDock::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);

    emit sgnlClose();
}
/*****************************************************************************/
void ScoresDock::Clear()
{
    tableScores->setRowCount(0);
}
/*****************************************************************************/
void ScoresDock::SetPlayers(const QMap<Place, Identity> &players)
{
    QStringList header;

    tableScores->setColumnCount(4);

    for (std::uint8_t i = 0; i < 4; i++)
    {
        Place place(i);
        QMap<Place, Identity>::const_iterator iter = players.find(place);
        if (iter != players.end())
        {
            header += QString(iter.value().nickname.c_str());
        }
        else
        {
            header += "";
        }
    }
    tableScores->setHorizontalHeaderLabels(header);
}
/*****************************************************************************/
void ScoresDock::SetNewScore(const Points &points, const Tarot::Bid &bid)
{
    mScore.AddPoints(points, bid, 4U);

    int  n = tableScores->rowCount() + 1;
    tableScores->setRowCount(n);

    for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(tableScores->columnCount()); i++)
    {
        int total = mScore.GetTotalPoints(Place(i));
        QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(total));
        tableScores->setItem(n - 1, i, newItem);
    }
}


//=============================================================================
// Fin du fichier ScoresDock.cpp
//=============================================================================
