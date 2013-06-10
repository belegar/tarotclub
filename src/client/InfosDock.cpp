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
#include "textes.h"

/*****************************************************************************/
InfosDock::InfosDock(QWidget *parent)
    : QDockWidget(trUtf8("Informations"), parent)
{
    ui.setupUi(this);
}
/*****************************************************************************/
void InfosDock::clear()
{
    ui.donneVar->setText("");
    ui.preneurVar->setText("");
    ui.contratVar->setText("");
}
/*****************************************************************************/
void InfosDock::setDonne(int n)
{
    ui.donneVar->setText(QString("<b>") + QString::number(n) + QString("</b>"));
}
/*****************************************************************************/
void InfosDock::setPreneur(QString preneur)
{
    ui.preneurVar->setText("<b>" + preneur + "</b>");
}
/*****************************************************************************/
void InfosDock::setContrat(Contract cont)
{
    QString mot;

    if (cont == GARDE_CONTRE)
    {
        mot = STR_GARDE_CONTRE;
    }
    else if (cont == GARDE_SANS)
    {
        mot = STR_GARDE_SANS;
    }
    else if (cont == GARDE)
    {
        mot = STR_GARDE;
    }
    else if (cont == PRISE)
    {
        mot = STR_PRISE;
    }
    else
    {
        mot = STR_PASSE;
    }
    ui.contratVar->setText("<b>" + mot + "</b>");
}
/*****************************************************************************/
void InfosDock::printStats(Deck::Statistics *stats)
{
    QString buffer, tmp;

    buffer += trUtf8("Atouts: ") + tmp.setNum(stats->atouts);
    buffer += trUtf8("\nAtouts majeurs: ") + tmp.setNum(stats->atoutsMajeurs);

    buffer += trUtf8("\nBouts: ") + tmp.setNum(stats->bouts);
    buffer += trUtf8("\nExcuse: ") + tmp.setNum(stats->excuse);
    buffer += trUtf8("\n21: ") + tmp.setNum(stats->vingtEtUn);
    buffer += trUtf8("\nPetit: ") + tmp.setNum(stats->petit);

    buffer += trUtf8("\nLongues: ") + tmp.setNum(stats->longues);
    buffer += trUtf8("\nMariages: ") + tmp.setNum(stats->mariages);
    buffer += trUtf8("\nSequences: ") + tmp.setNum(stats->sequences);
    buffer += trUtf8("\nSingletons: ") + tmp.setNum(stats->singletons);

    buffer += trUtf8("\nTrèfles: ") + tmp.setNum(stats->trefles);
    buffer += trUtf8("\nCarreaux: ") + tmp.setNum(stats->carreaux);
    buffer += trUtf8("\nPics: ") + tmp.setNum(stats->pics);
    buffer += trUtf8("\nCoeurs: ") + tmp.setNum(stats->coeurs);

    buffer += trUtf8("\nRois: ") + tmp.setNum(stats->rois);
    buffer += trUtf8("\nCoupes: ") + tmp.setNum(stats->coupes);
    buffer += trUtf8("\nDames: ") + tmp.setNum(stats->dames);
    buffer += trUtf8("\nCavaliers: ") + tmp.setNum(stats->cavaliers);
    buffer += trUtf8("\nValets: ") + tmp.setNum(stats->valets);

    ui.statsEdit->setPlainText(buffer);
}

//=============================================================================
// Fin du fichier InfosDock.cpp
//=============================================================================
