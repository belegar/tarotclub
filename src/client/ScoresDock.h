/*=============================================================================
 * TarotClub - ScoresDock.h
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

#include <QDockWidget>
#include <QTableWidget>
#include "../defines.h"

/*****************************************************************************/
class ScoresDock : public QDockWidget
{
   Q_OBJECT

private:
   QTableWidget *tableScores;

public:
   ScoresDock( QWidget *parent );

   void clear();
   void setOptions( GameOptions *options );
   void setNewScore( int *scores, int nbJoueurs );

protected:
   void closeEvent( QCloseEvent * e );

signals:
   void sgnlClose();

};

//=============================================================================
// Fin du fichier ScoresDock.h
//=============================================================================
