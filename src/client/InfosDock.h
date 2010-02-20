/*=============================================================================
 * TarotClub - InfosDock.h
 *=============================================================================
 * Fenêtre "dockable" où sont stockées quelques informations du jeu en cours.
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

#ifndef INFOSDOCK_H
#define INFOSDOCK_H

// Includes Qt
#include <QDockWidget>
#include <QLabel>
#include <ui_InfosDockUI.h>

// Includes locales
#include "../defines.h"

/*****************************************************************************/
class InfosDock : public QDockWidget
{
private:
   Ui::infosDock  ui;

public:
   InfosDock( QWidget *parent );

   void clear();
   void setContrat( Contrat c );
   void setPreneur( QString preneur );
   void setDonne( int n );

};

#endif // INFOSDOCK_H

//=============================================================================
// Fin du fichier InfosDock.h
//=============================================================================
