/*=============================================================================
 * Tarot Club - ResultWindow.h
 *=============================================================================
 * ResultWindow : affiche le résultat d'une donne
 *=============================================================================
 * Tarot Club est un jeu de Tarot français
 * Copyright (C) 2003-2005  Anthony Rabine
 * anthony@ooso.org
 * http://tarotclub.ooso.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *=============================================================================
 */

#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H

// Includes Qt
#include <QDialog>

// Includes locales
#include "../defines.h"
#include "ui/ui_ResultUI.h"

/*****************************************************************************/
class ResultWindow : public QDialog
{
	Q_OBJECT

private:
    Ui::ResultUI ui;
public:
	ResultWindow( QWidget* parent = 0, Qt::WFlags fl = 0 );

	void	setCalcul(  ScoreInfos *score, GameInfos *infos );

};

#endif // RESULTWINDOW_H

//=============================================================================
// Fin du fichier ResultWindow.h
//=============================================================================


