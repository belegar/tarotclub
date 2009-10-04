/*=============================================================================
 * Tarot Club - ScoresDock.h
 *=============================================================================
 * Fenêtre dockable affichant les scores des joueurs
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

#include <QDockWidget>
#include <QTableWidget>
#include "defines.h"

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
