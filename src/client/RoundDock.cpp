/*=============================================================================
 * TarotClub - RoundDock.cpp
 *=============================================================================
 * Fenêtre dockable affichant les scores des joueurs
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

#include <QHeaderView>
#include "RoundDock.h"

/*****************************************************************************/
RoundDock::RoundDock( QWidget *parent )
    : QDockWidget( parent )
{
   ui.setupUi(this);
}
/*****************************************************************************/
void RoundDock::closeEvent( QCloseEvent *e )
{
   emit sgnlClose();
}


//=============================================================================
// Fin du fichier RoundDock.cpp
//=============================================================================