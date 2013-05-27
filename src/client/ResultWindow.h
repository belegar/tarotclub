/*=============================================================================
 * TarotClub - ResultWindow.h
 *=============================================================================
 * ResultWindow : affiche le résultat d'une donne
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

#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H

// Includes Qt
#include <QDialog>

// Local includes
#include "../defines.h"
#include "../GameState.h"
#include "../ScoreInfo.h"
#include <ui_ResultUI.h>

/*****************************************************************************/
class ResultWindow : public QDialog
{
    Q_OBJECT

private:
    Ui::ResultUI ui;
public:
    ResultWindow(QWidget *parent = 0);

    void SetResult(ScoreInfo &score, GameState &info);

};

#endif // RESULTWINDOW_H

//=============================================================================
// Fin du fichier ResultWindow.h
//=============================================================================


