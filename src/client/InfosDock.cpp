/*=============================================================================
 * TarotClub - InfosDock.h
 *=============================================================================
 * Dock window that showing various current game information
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

// Includes Qt
#include <QDockWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

// Includes locales
#include "InfosDock.h"
#include "../Tools.h"

/*****************************************************************************/
InfosDock::InfosDock(QWidget *parent)
    : QDockWidget(trUtf8("Informations"), parent)
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
void InfosDock::Clear()
{
    ui.donneVar->setText("");
    ui.preneurVar->setText("");
    ui.contratVar->setText("");

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
void InfosDock::SetDealNumber(int n)
{
    if (n < 0)
    {
        ui.donneVar->setText("");
    }
    else
    {
        ui.donneVar->setText(QString("<b>") + QString::number(n) + QString("</b>"));
    }
}
/*****************************************************************************/
void InfosDock::SetTaker(QString &name, Place place)
{
    ui.preneurVar->setText("<b>" + name + "</b>" + " (" + Util::ToString(place) + ")");
}
/*****************************************************************************/
void InfosDock::SetContract(Contract contract)
{
    ui.contratVar->setText("<b>" + Util::ToString(contract) + "</b>");
}
/*****************************************************************************/
void InfosDock::PrintStats(Deck::Statistics &stats)
{
    QString buffer, tmp;

    buffer += trUtf8("Trumps: ") + tmp.setNum(stats.trumps);
    buffer += "\n" + trUtf8("Major trumps: ") + tmp.setNum(stats.atoutsMajeurs);

    buffer += "\n" + trUtf8("Oudlers: ") + tmp.setNum(stats.oudlers);
    buffer += "\n" + trUtf8("Fool: ") + tmp.setNum(stats.excuse);
    buffer += "\n" + trUtf8("21 of trump: ") + tmp.setNum(stats.vingtEtUn);
    buffer += "\n" + trUtf8("1 of trump: ") + tmp.setNum(stats.petit);

    buffer += "\n" + trUtf8("Long suits: ") + tmp.setNum(stats.longues);
    buffer += "\n" + trUtf8("Weddings: ") + tmp.setNum(stats.mariages);
    buffer += "\n" + trUtf8("Sequences: ") + tmp.setNum(stats.sequences);
    buffer += "\n" + trUtf8("Singletons: ") + tmp.setNum(stats.singletons);

    buffer += "\n" + trUtf8("Clubs: ") + tmp.setNum(stats.trefles);
    buffer += "\n" + trUtf8("Diamonds: ") + tmp.setNum(stats.carreaux);
    buffer += "\n" + trUtf8("Spades: ") + tmp.setNum(stats.pics);
    buffer += "\n" + trUtf8("Hearts: ") + tmp.setNum(stats.coeurs);

    buffer += "\n" + trUtf8("Kings: ") + tmp.setNum(stats.rois);
    buffer += "\n" + trUtf8("Voids: ") + tmp.setNum(stats.coupes);
    buffer += "\n" + trUtf8("Queens: ") + tmp.setNum(stats.dames);
    buffer += "\n" + trUtf8("Knights: ") + tmp.setNum(stats.cavaliers);
    buffer += "\n" + trUtf8("Jacks: ") + tmp.setNum(stats.valets);

    ui.statsEdit->setPlainText(buffer);
}
/*****************************************************************************/
void InfosDock::AddRound(Game &info, Place p, const QString &txt)
{
    ui.tableWidget->item(info.trickCounter, p)->setText(txt);
}
/*****************************************************************************/
void InfosDock::SelectWinner(Game &info, Place p)
{
    QBrush brush(Qt::darkGreen);
    ui.tableWidget->item(info.trickCounter-1, p)->setBackground(brush);
}
/*****************************************************************************/
void InfosDock::SelectFirstPlayer(int turn, Place p)
{
    QBrush brush(Qt::lightGray);
    ui.tableWidget->item(turn, p)->setBackground(brush);
}

//=============================================================================
// Fin du fichier InfosDock.cpp
//=============================================================================
