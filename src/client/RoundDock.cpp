/*=============================================================================
 * TarotClub - RoundDock.cpp
 *=============================================================================
 * Fenêtre dockable affichant les plis réalisés à chaque tour de jeu
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
   int i, j;
   ui.setupUi(this);

   ui.tableWidget->setRowCount(18);

   for( i=0; i<5; i++) {
      for( j=0; j<18; j++) {
         ui.tableWidget->setItem(j, i, new QTableWidgetItem());
      }
   }
}
/*****************************************************************************/
void RoundDock::closeEvent( QCloseEvent *e )
{
   Q_UNUSED(e);
   emit sgnlClose();
}
/*****************************************************************************/
void RoundDock::clear()
{
   int i, j;
   QBrush brush(Qt::white);

   for( i=0; i<5; i++) {
      for( j=0; j<18; j++) {
         ui.tableWidget->item(j, i)->setText("");
         ui.tableWidget->item(j, i)->setBackground(brush);
      }
   }
}
/*****************************************************************************/
void RoundDock::addRound(int turn, Place p, QString txt)
{
   ui.tableWidget->item(turn, p+1)->setText(txt);
}
/*****************************************************************************/
void RoundDock::selectWinner(int turn, Place p)
{
   QBrush brush(Qt::darkGreen);
   ui.tableWidget->item(turn, p+1)->setBackground(brush);
}
/*****************************************************************************/
void RoundDock::pointsToTaker(int turn, float points)
{
    QString txt;

    txt.setNum(points);
    ui.tableWidget->item(turn, 0)->setText(txt);
}
/*****************************************************************************/
void RoundDock::selectFirstPlayer(int turn, Place p)
{
   QBrush brush(Qt::lightGray);
   ui.tableWidget->item(turn, p+1)->setBackground(brush);
}

//=============================================================================
// Fin du fichier RoundDock.cpp
//=============================================================================
