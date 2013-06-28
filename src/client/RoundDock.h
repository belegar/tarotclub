/*=============================================================================
 * TarotClub - RoundDock.h
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

#include <QDockWidget>
#include <QTableWidget>
#include "../defines.h"
#include "../Game.h"
#include "ui_RoundUI.h"

/*****************************************************************************/
class RoundDock : public QDockWidget
{
    Q_OBJECT

private:
    Ui::RoundUI ui;

public:
    RoundDock(QWidget *parent);

    void Clear();
    void AddRound(Game &info, Place p, QString txt);
    void SelectWinner(Game &info, Place p);
    void SelectFirstPlayer(int turn, Place p);

protected:
    void closeEvent(QCloseEvent *e);

signals:
    void sgnlClose();

};

//=============================================================================
// Fin du fichier RoundDock.h
//=============================================================================
