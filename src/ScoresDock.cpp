/*=============================================================================
 * Tarot Club - ScoresDock.cpp
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

#include <QHeaderView>
#include "ScoresDock.h"

/*****************************************************************************/
ScoresDock::ScoresDock( QWidget *parent )
    : QDockWidget( tr("Scores"), parent )
{
   tableScores = new QTableWidget( this );
   tableScores->setSelectionMode(QAbstractItemView::NoSelection);
   QHeaderView *hv = tableScores->horizontalHeader();
   hv->setResizeMode(QHeaderView::Stretch);
   setWidget(tableScores);
}
/*****************************************************************************/
void ScoresDock::closeEvent( QCloseEvent *e )
{
   emit sgnlClose();
}
/*****************************************************************************/
void ScoresDock::clear()
{
   tableScores->setRowCount(0);
}
/*****************************************************************************/
void ScoresDock::setOptions( GameOptions *options )
{
   int i;
   QStringList header;

   tableScores->setColumnCount( options->nbPlayers );

   // Les noms des joueurs
   for( i=0; i<options->nbPlayers; i++ ) {
      header += options->identities[i].name;
   }
   tableScores->setHorizontalHeaderLabels( header );
}
/*****************************************************************************/
void ScoresDock::setNewScore( int *scores, int nbJoueurs )
{
   int n;

   n = tableScores->rowCount()+1;
   tableScores->setRowCount(n);

   for( int i=0; i<nbJoueurs; i++ ) {
      QTableWidgetItem *newItem = new QTableWidgetItem( QString::number(scores[i]) );
      tableScores->setItem(n-1, i, newItem);
   }
}


//=============================================================================
// Fin du fichier ScoresDock.cpp
//=============================================================================
