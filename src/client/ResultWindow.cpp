/*=============================================================================
 * TarotClub - ResultWindow.cpp
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

#include "ResultWindow.h"
#include "textes.h"

/*****************************************************************************/
ResultWindow::ResultWindow( QWidget* parent, Qt::WFlags fl )
	: QDialog( parent, fl )
{
    ui.setupUi(this);
}
/*****************************************************************************/
void ResultWindow::setCalcul( ScoreInfos *score, GameInfos *infos )
{
   QString result_str;

	// On affiche les caractéristiques de la partie
   if( infos->preneur == SUD ) {
		ui.lblPreneur->setText(STR_SUD);
   } else if( infos->preneur == EST ) {
		ui.lblPreneur->setText(STR_EST);
   }else if( infos->preneur == NORD ) {
		ui.lblPreneur->setText(STR_NORD);
   } else if( infos->preneur == OUEST ) {
		ui.lblPreneur->setText(STR_OUEST);
   } else {
      ui.lblPreneur->setText(STR_NORD_O);
   } 

   if( infos->contrat == PRISE ) {
		ui.lblContrat->setText(STR_PRISE);
   } else if( infos->contrat == GARDE ) {
		ui.lblContrat->setText(STR_GARDE);
   } else if( infos->contrat == GARDE_SANS ) {
		ui.lblContrat->setText(STR_GARDE_SANS);
   } else {
		ui.lblContrat->setText(STR_GARDE_CONTRE);
   }

   ui.lblNbBouts->setNum( score->bouts );
   ui.lblPoints->setNum( (int)score->attaque );

   if( (int)(score->attaque) >= score->pointsAFaire ) {
      result_str = QString("<font color=\"#0D851B\">") + STR_WIN;
	} else {
      result_str = QString("<font color=\"red\">") + STR_LOSE;
	}
   ui.lblResultat->setText(result_str + QString().setNum( score->difference ) + 
                           STR_POINTS + QString("</font>"));

	ui.lblGains->setNum( score->difference );
   ui.lblPetit->setNum( score->points_petit_au_bout );
   ui.lblMultiple->setNum( score->multiplicateur );
   ui.lblPoignee->setNum( score->points_poignee );
   ui.lblChelem->setNum( score->points_chelem );
   ui.lblTotalDefense->setText( QString().setNum( score->defense ) + STR_POINTS );
	ui.lblTotalAttaque->setText( QString().setNum( score->defense*(-3) ) + STR_POINTS );
}


//=============================================================================
// Fin du fichier ResultWindow.cpp
//=============================================================================
