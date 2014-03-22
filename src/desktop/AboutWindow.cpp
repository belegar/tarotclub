/*=============================================================================
 * TarotClub - AboutWindow.cpp
 *=============================================================================
 * Custom "about" window
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

#include <QString>
#include "AboutWindow.h"
#include "Common.h"

/*****************************************************************************/
AboutWindow::AboutWindow(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    ui.version->setText(QString(TAROT_TITRE) + " " +  QString(TAROT_VERSION));
    ui.nom->setText("<" + QString(TAROT_VNAME) + ">");

}

//=============================================================================
// End of file AboutWindow.cpp
//=============================================================================
