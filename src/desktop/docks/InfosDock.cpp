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
#include "Common.h"

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
void InfosDock::SetPlayers(const QMap<Place, Identity> &players)
{
    QStringList header;

    ui.tableWidget->setColumnCount(players.size());

    QMapIterator<Place, Identity> i(players);
    while (i.hasNext())
    {
        i.next();
        header += i.value().name.data();
    }
    ui.tableWidget->setHorizontalHeaderLabels(header);
}
/*****************************************************************************/
void InfosDock::SetDealNumber(std::uint32_t n)
{
    if (n == 0U)
    {
        ui.donneVar->setText("");
    }
    else
    {
        ui.donneVar->setText(QString("<b>") + QString::number(n) + QString("</b>"));
    }
}
/*****************************************************************************/
void InfosDock::SetTaker(const QString &name, Place place)
{
    ui.preneurVar->setText("<b>" + name + "</b>" + " (" + QString(place.ToString().data()) + ")");
}
/*****************************************************************************/
void InfosDock::SetContract(Contract contract)
{
    ui.contratVar->setText("<b>" + QString(contract.ToString().data()) + "</b>");
}
/*****************************************************************************/
void InfosDock::PrintStats(const Deck::Statistics &stats)
{
    QString buffer, tmp;

    buffer += trUtf8("Trumps: ") + tmp.setNum(stats.trumps);
    buffer += "\n" + trUtf8("Major trumps: ") + tmp.setNum(stats.majorTrumps);

    buffer += "\n" + trUtf8("Oudlers: ") + tmp.setNum(stats.oudlers);
    buffer += "\n" + trUtf8("Fool: ") + tmp.setNum(stats.fool);
    buffer += "\n" + trUtf8("21 of trump: ") + tmp.setNum(stats.bigTrump);
    buffer += "\n" + trUtf8("1 of trump: ") + tmp.setNum(stats.littleTrump);

    buffer += "\n" + trUtf8("Long suits: ") + tmp.setNum(stats.longSuits);
    buffer += "\n" + trUtf8("Weddings: ") + tmp.setNum(stats.weddings);
    buffer += "\n" + trUtf8("Sequences: ") + tmp.setNum(stats.sequences);
    buffer += "\n" + trUtf8("Singletons: ") + tmp.setNum(stats.singletons);

    buffer += "\n" + trUtf8("Clubs: ") + tmp.setNum(stats.clubs);
    buffer += "\n" + trUtf8("Diamonds: ") + tmp.setNum(stats.diamonds);
    buffer += "\n" + trUtf8("Spades: ") + tmp.setNum(stats.spades);
    buffer += "\n" + trUtf8("Hearts: ") + tmp.setNum(stats.hearts);

    buffer += "\n" + trUtf8("Kings: ") + tmp.setNum(stats.kings);
    buffer += "\n" + trUtf8("Voids: ") + tmp.setNum(stats.cuts);
    buffer += "\n" + trUtf8("Queens: ") + tmp.setNum(stats.queens);
    buffer += "\n" + trUtf8("Knights: ") + tmp.setNum(stats.knights);
    buffer += "\n" + trUtf8("Jacks: ") + tmp.setNum(stats.jacks);

    ui.statsEdit->setPlainText(buffer);
}
/*****************************************************************************/
void InfosDock::AddRound(std::uint8_t trickCounter, Place p, const QString &txt)
{
    ui.tableWidget->item(trickCounter, p.Value())->setText(txt);
}
/*****************************************************************************/
void InfosDock::SelectWinner(std::uint8_t trickCounter, Place p)
{
    QBrush brush(Qt::darkGreen);
    ui.tableWidget->item(trickCounter - 1, p.Value())->setBackground(brush);
}
/*****************************************************************************/
void InfosDock::SelectFirstPlayer(int turn, Place p)
{
    QBrush brush(Qt::lightGray);
    ui.tableWidget->item(turn, p.Value())->setBackground(brush);
}

//=============================================================================
// Fin du fichier InfosDock.cpp
//=============================================================================
