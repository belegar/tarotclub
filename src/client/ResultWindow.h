/*=============================================================================
 * TarotClub - ResultWindow.h
 *=============================================================================
 * ResultWindow : affiche le résultat d'une donne
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 *=============================================================================
 */

#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H

// Includes Qt
#include <QDialog>

// Includes locales
#include "../defines.h"
#include <ui_ResultUI.h>

/*****************************************************************************/
class ResultWindow : public QDialog
{
	Q_OBJECT

private:
    Ui::ResultUI ui;
public:
    ResultWindow( QWidget* parent = 0);

	void	setCalcul(  ScoreInfos *score, GameInfos *infos );

};

#endif // RESULTWINDOW_H

//=============================================================================
// Fin du fichier ResultWindow.h
//=============================================================================


