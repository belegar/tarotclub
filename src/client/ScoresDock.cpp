/*=============================================================================
 * TarotClub - ScoresDock.cpp
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
#include "ScoresDock.h"

/*****************************************************************************/
ScoresDock::ScoresDock( QWidget *parent )
    : QDockWidget( trUtf8("Scores"), parent )
{
   tableScores = new QTableWidget( this );
   tableScores->setSelectionMode(QAbstractItemView::NoSelection);
   tableScores->setEditTriggers(QAbstractItemView::NoEditTriggers);
   tableScores->setDragEnabled(false);
   tableScores->setSelectionMode(QAbstractItemView::SingleSelection);
   tableScores->setSelectionBehavior(QAbstractItemView::SelectRows);

   QHeaderView *hv = tableScores->horizontalHeader();
   hv->setSectionResizeMode(QHeaderView::Stretch);
   setWidget(tableScores);
}
/*****************************************************************************/
void ScoresDock::closeEvent( QCloseEvent *e )
{
   Q_UNUSED(e);

   emit sgnlClose();
}
/*****************************************************************************/
void ScoresDock::clear()
{
   tableScores->setRowCount(0);
}
/*****************************************************************************/
void ScoresDock::setPlayers( QList<Identity> &players )
{
   int i;
   QStringList header;

   tableScores->setColumnCount(NB_PLAYERS);

   // Les noms des joueurs
   for( i=0; i<players.size(); i++ ) {
      header += players.at(i).name;
   }
   tableScores->setHorizontalHeaderLabels( header );
}
/*****************************************************************************/
void ScoresDock::setNewScore(Score *score)
{
   int n, total;

   n = tableScores->rowCount()+1;
   tableScores->setRowCount(n);

   for( int i=0; i<NB_PLAYERS; i++ ) {
      total = score->getTotalPoints((Place)i);
      QTableWidgetItem *newItem = new QTableWidgetItem( QString::number(total) );
      tableScores->setItem(n-1, i, newItem);
   }
}


//=============================================================================
// Fin du fichier ScoresDock.cpp
//=============================================================================
